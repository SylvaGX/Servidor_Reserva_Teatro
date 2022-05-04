#include "server.h"

/**
* Função para mandar dados para um cliente
* @param Client* client
* @param char** data
* @return int - retorna o restulado do envio dos dados
*/
int sendData(Client* client, std::string data) {
	char sendMsg[1024];
	std::string logMsg{};
	std::thread::id myid = std::this_thread::get_id();
	std::stringstream ss{};
	ss << myid;
	std::string threadId = ss.str();

	int ws_result = 0;

	ZeroMemory(sendMsg, 1024);
	strcpy(sendMsg, data.c_str());

	ws_result = send(client->getSocket(), sendMsg, strlen(sendMsg) + 1, 0); // Função para enviar dados para o cliente

	// Hanlde errors
	if (ws_result < 0)
	{

		logMsg = "Connect Error! Thread: " + threadId; // Copiar a string para a variável

		Log::WarningLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
		return 1;
	}

	return ws_result;
}

/**
* Função para atualizar as visitas de um determinado teatro no ficheiro de teatros.csv
* @param Teatro** teatros
* @param int id
* @param Client* client
*/
void updateVisitas(std::vector<Teatro> teatros, int id) {
	
	std::ofstream f;
	std::string logMsg{};
	std::thread::id myid = std::this_thread::get_id();
	std::stringstream ss{};
	ss << myid;
	std::string threadId = ss.str();

	f.open("teatros.csv"); // Abrir o ficheiro teatros.csv

	// Verificar se abriu o ficheiro corretamente
	if (f)
	{
		logMsg = "A incrementar o numero de visitados. Thread: " + threadId; // Copiar a string para a variável

		Log::InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal

		for (auto &i : teatros)
		{
			if (id == i.getId())
			{
				f << i.getId() << "," << i.getNome() << "," << i.getEspetaculos() << "," << i.getLocalizacao() << "," << (i.getVisitas() + 1) << "\n";
			}
			else
			{
				f << i.getId() << "," << i.getNome() << "," << i.getEspetaculos() << "," << i.getLocalizacao() << "," << i.getVisitas() << "\n";
			}
		}

		f.close(); // Fechar o ficheiro
	}
	else
	{
		logMsg = "Erro ao incrementar o número de visitas. Thread: " + threadId; // Copiar a string para a variável

		Log::WarningLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
	}
}

/**
* Função para comparar os ids dos teatros
* @param const void* a
* @param const void* b
*/
int compareTeatrosVisitados(const void* a, const void* b)
{
	return (*(int*)a - *(int*)b);
}

/**
* Função principal que corre por thread para que possa disponibilizar acesso por cliente. 
* Cada cliente inicializado é criada uma thread que corre esta função para gerenciar o mesmo cliente.
* @param Client* client
*/
void __stdcall ClientSession(Client* client) {
	char sendMsg[1024];
	char revMsg[1024];
	std::string logMsg{};
	std::thread::id myid = std::this_thread::get_id();
	std::stringstream ss{};
	ss << myid;
	std::string threadId = ss.str();

	int bytesReceived = 0;
	int ws_result = 0;

	logMsg = "User connected. Thread: " + threadId; // Copiar a string para a variável

	Log::InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal

	// Loop da thread para ficar atento aos pedidos do cliente
	while (TRUE)
	{
		ZeroMemory(revMsg, 1024); // Limpar a variavel

		bytesReceived = recv(client->getSocket(), revMsg, 1024, 0); // Ficar à espera de uma resposta do cliente

		// Erro inesperado do cliente
		if (bytesReceived == SOCKET_ERROR) {

			logMsg = "Receive error! Thread: " + threadId; // Copiar a string para a variável

			Log::WarningLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
			break;
		}

		// Cliente desconectado
		if (bytesReceived == 0) {

			logMsg = "Cliente Desconectado! Thread: " + threadId; // Copiar a string para a variável

			Log::InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
			break;
		}

		// A thread recebeu informação sem erro
		if (bytesReceived > 0)
		{
			//Verificar qual o pedido do cliente

			// Se fizer um pedido de TEATRO
			// Iremos mandar os teatros ao cliente
			if (strcmp(revMsg, "TEATRO") == 0) {

				logMsg = "Pedido teatro. Thread: " + threadId; // Copiar a string para a variável

				Log::InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal

				ZeroMemory(revMsg, 1024); // Limpar a variável
				ZeroMemory(sendMsg, 1024); // Limpar a variável

				strcpy(sendMsg, "100 OK"); // Copiar a string para a variável

				sendData(client, sendMsg); // Mandar dados para o cliente

				// Bloquear o mutex das cidades
				std::unique_lock<std::mutex> lckTeatro(Teatro::getMutex());
				// Bloquear o mutex dos teatros
				std::unique_lock<std::mutex> lckCidade(Cidade::getMutex());

				float lat = 0.0f;
				float longi = 0.0f;

				logMsg = "A obter a localizacao. Thread: " + threadId; // Copiar a string para a variável

				Log::InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal

				auto itA = Cidade::find(Cidade::getCidades().begin(), Cidade::getCidades().end(), client->getLocation()); // obter o index da cidade do cliente

				lat = (*itA).getLat(); // Obter a latitude da cidade
				longi = (*itA).getLong(); // Obter a longitude da cidade

				Teatro::InitTeatros(lat, longi); // Ler os teatros do ficheiro e guardar numa estrutura

				int value = 0;
				std::string str{};

				logMsg = "A enviar teatros.... Thread: " + threadId; // Copiar a string para a variável

				Log::InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal

				bool auxId;
				// Enviar os teatros para o cliente
				for (auto& teatro : Teatro::getTeatros())
				{
					/*-------Loop------*/

					auxId = client->contains(client->getTeatrosVisitados(), teatro.getId());

					if (!auxId)
					{
						value = teatro.getId(); // Obter o id do teatro

						str.assign(std::to_string(value)); // Converter inteiro para string

						sendData(client, str); // Mandar dados para o cliente

						ZeroMemory(revMsg, 1024);
						bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

						if (bytesReceived == SOCKET_ERROR) {

							logMsg = "Receive error! Thread: " + threadId;

							Log::WarningLog(logMsg);
							break;
						}

						if (bytesReceived == 0) {

							logMsg = "Cliente Desconectado! Thread: " + threadId;

							Log::InfoLog(logMsg);
							break;
						}

						if (bytesReceived > 0)
						{
							if (strcmp(revMsg, "100 OK") == 0) {

								sendData(client, teatro.getNome());

								ZeroMemory(revMsg, 1024);
								bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

								if (bytesReceived == SOCKET_ERROR) {

									logMsg = "Receive error! Thread: " + threadId;

									Log::WarningLog(logMsg);
									break;
								}

								if (bytesReceived == 0) {

									logMsg = "Cliente Desconectado! Thread: " + threadId;

									Log::InfoLog(logMsg);
									break;
								}

								if (bytesReceived > 0)
								{
									if (strcmp(revMsg, "100 OK") == 0) {

										value = teatro.getEspetaculos();

										str.assign(std::to_string(value)); // Converter inteiro para string

										sendData(client, str);

										ZeroMemory(revMsg, 1024);
										bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

										if (bytesReceived == SOCKET_ERROR) {

											logMsg = "Receive error! Thread: " + threadId;

											Log::WarningLog(logMsg);
											break;
										}

										if (bytesReceived == 0) {

											logMsg = "Cliente Desconectado! Thread: " + threadId;

											Log::InfoLog(logMsg);
											break;
										}

										if (bytesReceived > 0)
										{
											if (strcmp(revMsg, "100 OK") == 0) {

												sendData(client, teatro.getLocalizacao());

												ZeroMemory(revMsg, 1024);
												bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

												if (bytesReceived == SOCKET_ERROR) {

													logMsg = "Receive error! Thread: " + threadId;

													Log::WarningLog(logMsg);
													break;
												}

												if (bytesReceived == 0) {

													logMsg = "Cliente Desconectado! Thread: " + threadId;

													Log::InfoLog(logMsg);
													break;
												}

												if (bytesReceived > 0)
												{
													if (strcmp(revMsg, "100 OK") == 0) {

														value = teatro.getVisitas();

														str.assign(std::to_string(value)); // Converter inteiro para string

														sendData(client, str);

														ZeroMemory(revMsg, 1024);
														bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

														if (bytesReceived == SOCKET_ERROR) {

															logMsg = "Receive error! Thread: " + threadId;

															Log::WarningLog(logMsg);
															break;
														}

														if (bytesReceived == 0) {

															logMsg = "Cliente Desconectado! Thread: " + threadId;

															Log::InfoLog(logMsg);
															break;
														}

														if (bytesReceived > 0)
														{
															if (strcmp(revMsg, "100 OK") == 0) {


															}
														}
													}
													else {

														logMsg = "Erro de protocolo de comunicaçâo! Thread: " + threadId;

														Log::InfoLog(logMsg);
													}
												}
											}
											else {

												logMsg = "Erro de protocolo de comunicaçâo! Thread: " + threadId;

												Log::InfoLog(logMsg);
											}
										}
									}
									else {

										logMsg = "Erro de protocolo de comunicaçâo! Thread: " + threadId;

										Log::InfoLog(logMsg);
									}
								}
							}
							else {

								logMsg = "Erro de protocolo de comunicaçâo! Thread: " + threadId;

								Log::InfoLog(logMsg);
							}
						}
					}
					/*-------Loop------*/
				}

				sendData(client, "END");

				ZeroMemory(revMsg, 1024);
				bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

				if (bytesReceived == SOCKET_ERROR) {

					logMsg = "Receive error! Thread: " + threadId;

					Log::WarningLog(logMsg);
					break;
				}

				if (bytesReceived == 0) {

					logMsg = "Cliente Desconectado! Thread: " + threadId;

					Log::InfoLog(logMsg);
					break;
				}

				//Enviar o número de teatros que o cliente visitou
				sendData(client, std::to_string(client->getTeatrosVisitados().size()));

				ZeroMemory(revMsg, 1024);
				bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

				if (bytesReceived == SOCKET_ERROR) {

					logMsg = "Receive error! Thread: " + threadId;

					Log::WarningLog(logMsg);
					break;
				}

				if (bytesReceived == 0) {

					logMsg = "Cliente Desconectado! Thread: " + threadId;

					Log::InfoLog(logMsg);
					break;
				}

				std::vector<Teatro>::iterator teatro;
				std::vector<Teatro>::iterator pEnd = Teatro::getTeatros().end();

				// Enviar os teatros visitados pelo cliente
				for (auto& i : client->getTeatrosVisitados())
				{

					teatro = std::find(Teatro::getTeatros().begin(), Teatro::getTeatros().end(), i);

					if (teatro != pEnd)
					{
						value = (*teatro).getId(); // Obter o id do teatro

						str.assign(std::to_string(value)); // Converter inteiro para string

						sendData(client, str); // Mandar dados para o cliente

						ZeroMemory(revMsg, 1024);
						bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

						if (bytesReceived == SOCKET_ERROR) {

							logMsg = "Receive error! Thread: " + threadId;

							Log::WarningLog(logMsg);
							break;
						}

						if (bytesReceived == 0) {

							logMsg = "Cliente Desconectado! Thread: " + threadId;

							Log::InfoLog(logMsg);
							break;
						}

						if (bytesReceived > 0)
						{
							if (strcmp(revMsg, "100 OK") == 0) {

								sendData(client, (*teatro).getNome());

								ZeroMemory(revMsg, 1024);
								bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

								if (bytesReceived == SOCKET_ERROR) {

									logMsg = "Receive error! Thread: " + threadId;

									Log::WarningLog(logMsg);
									break;
								}

								if (bytesReceived == 0) {

									logMsg = "Cliente Desconectado! Thread: " + threadId;

									Log::InfoLog(logMsg);
									break;
								}

								if (bytesReceived > 0)
								{
									if (strcmp(revMsg, "100 OK") == 0) {

										value = (*teatro).getEspetaculos();

										str.assign(std::to_string(value)); // Converter inteiro para string

										sendData(client, str);

										ZeroMemory(revMsg, 1024);
										bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

										if (bytesReceived == SOCKET_ERROR) {

											logMsg = "Receive error! Thread: " + threadId;

											Log::WarningLog(logMsg);
											break;
										}

										if (bytesReceived == 0) {

											logMsg = "Cliente Desconectado! Thread: " + threadId;

											Log::InfoLog(logMsg);
											break;
										}

										if (bytesReceived > 0)
										{
											if (strcmp(revMsg, "100 OK") == 0) {

												sendData(client, (*teatro).getLocalizacao());

												ZeroMemory(revMsg, 1024);
												bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

												if (bytesReceived == SOCKET_ERROR) {

													logMsg = "Receive error! Thread: " + threadId;

													Log::WarningLog(logMsg);
													break;
												}

												if (bytesReceived == 0) {

													logMsg = "Cliente Desconectado! Thread: " + threadId;

													Log::InfoLog(logMsg);
													break;
												}

												if (bytesReceived > 0)
												{
													if (strcmp(revMsg, "100 OK") == 0) {

														value = (*teatro).getVisitas();

														str.assign(std::to_string(value)); // Converter inteiro para string

														sendData(client, str);

														ZeroMemory(revMsg, 1024);
														bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

														if (bytesReceived == SOCKET_ERROR) {

															logMsg = "Receive error! Thread: " + threadId;

															Log::WarningLog(logMsg);
															break;
														}

														if (bytesReceived == 0) {

															logMsg = "Cliente Desconectado! Thread: " + threadId;

															Log::InfoLog(logMsg);
															break;
														}

														if (bytesReceived > 0)
														{
															if (strcmp(revMsg, "100 OK") == 0) {


															}
														}
													}
													else {

														logMsg = "Erro de protocolo de comunicaçâo! Thread: " + threadId;

														Log::InfoLog(logMsg);
													}
												}
											}
											else {

												logMsg = "Erro de protocolo de comunicaçâo! Thread: " + threadId;

												Log::InfoLog(logMsg);
											}
										}
									}
									else {

										logMsg = "Erro de protocolo de comunicaçâo! Thread: " + threadId;

										Log::InfoLog(logMsg);
									}
								}
							}
							else {

								logMsg = "Erro de protocolo de comunicaçâo! Thread: " + threadId;

								Log::InfoLog(logMsg);
							}
						}
					}
				}

				logMsg = "Teatros enviados. Thread: " + threadId;

				Log::InfoLog(logMsg);

				// Limpar a estrutura de teatros
				logMsg = "A limpar estrutura de teatros. Thread: " + threadId;

				Log::InfoLog(logMsg);

				// Limpar a estrutura de teatros
				Teatro::getTeatros().clear();
				Teatro::getTeatros().shrink_to_fit();

				logMsg = "Estrutura de teatros limpa com sucesso. Thread: " + threadId;

				Log::InfoLog(logMsg);

				sendData(client, "END");

				ZeroMemory(revMsg, 1024);
				bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

				if (bytesReceived == SOCKET_ERROR) {
					
					logMsg = "Receive error! Thread: " + threadId;

					Log::WarningLog(logMsg);
					break;
				}

				if (bytesReceived == 0) {
					
					logMsg = "Cliente Desconectado! Thread: " + threadId;

					Log::InfoLog(logMsg);
					break;
				}
				
				logMsg = "Pedido teatro finalizado. Thread: " + threadId;

				Log::InfoLog(logMsg);
				
			}

			// Se fizer um pedido de COMPRA
			// Iremos mandar os teatros não visitados pelo cliente
			// e esperar por uma resposta do cliente de qual é que
			// teatro já visitou
			if (strcmp(revMsg, "COMPRA") == 0) {

				logMsg = "Pedido compra. Thread: " + threadId;

				Log::InfoLog(logMsg);

				ZeroMemory(revMsg, 1024);
				ZeroMemory(sendMsg, 1024);

				strcpy(sendMsg, "100 OK");

				sendData(client, sendMsg);

				// Bloquear o mutex das cidades
				std::unique_lock<std::mutex> lckTeatro(Teatro::getMutex());
				// Bloquear o mutex dos teatros
				std::unique_lock<std::mutex> lckCidade(Cidade::getMutex());

				float lat = 0.0f;
				float longi = 0.0f;

				logMsg = "A obter a localizacao. Thread: " + threadId;

				Log::InfoLog(logMsg);

				auto itA = Cidade::find(Cidade::getCidades().begin(), Cidade::getCidades().end(), client->getLocation()); // obter o index da cidade do cliente

				lat = (*itA).getLat(); // Obter a latitude da cidade
				longi = (*itA).getLong(); // Obter a longitude da cidade

				Teatro::InitTeatros(lat, longi); // Ler os teatros do ficheiro e guardar numa estrutura
				
				int value = 0;
				std::string str{};

				logMsg = "A enviar teatros.... Thread: " + threadId;

				Log::InfoLog(logMsg);

				int auxId = -1;
				int isSair = 0;
				int isVisitado = 0;
				int isInTeatro = 0;

				// Enviar os teatros para o cliente
				for (auto &teatro : Teatro::getTeatros())
				{
					/*-------Loop------*/

					auxId = client->contains(client->getTeatrosVisitados(), teatro.getId());

					if (!auxId)
					{
						value = teatro.getId();

						str.assign(std::to_string(value)); // Converter inteiro para string

						sendData(client, str);

						ZeroMemory(revMsg, 1024);
						bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

						if (bytesReceived == SOCKET_ERROR) {
							
							logMsg = "Receive error! Thread: " + threadId;

							Log::WarningLog(logMsg);
							break;
						}

						if (bytesReceived == 0) {

							logMsg = "Cliente Desconectado! Thread: " + threadId;

							Log::InfoLog(logMsg);
							break;
						}

						if (bytesReceived > 0)
						{
							if (strcmp(revMsg, "100 OK") == 0) {

								sendData(client, teatro.getNome());

								ZeroMemory(revMsg, 1024);
								bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

								if (bytesReceived == SOCKET_ERROR) {

									logMsg = "Receive error! Thread: " + threadId;

									Log::WarningLog(logMsg);
									break;
								}

								if (bytesReceived == 0) {

									logMsg = "Cliente Desconectado! Thread: " + threadId;

									Log::InfoLog(logMsg);
									break;
								}

								if (bytesReceived > 0)
								{
									if (strcmp(revMsg, "100 OK") == 0) {

										value = teatro.getEspetaculos();

										str.assign(std::to_string(value)); // Converter inteiro para string

										sendData(client, str);

										ZeroMemory(revMsg, 1024);
										bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

										if (bytesReceived == SOCKET_ERROR) {
											
											logMsg = "Receive error! Thread: " + threadId;

											Log::WarningLog(logMsg);
											break;
										}

										if (bytesReceived == 0) {
											
											logMsg = "Cliente Desconectado! Thread: " + threadId;

											Log::InfoLog(logMsg);
											break;
										}

										if (bytesReceived > 0)
										{
											if (strcmp(revMsg, "100 OK") == 0) {

												sendData(client, teatro.getLocalizacao());

												ZeroMemory(revMsg, 1024);
												bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

												if (bytesReceived == SOCKET_ERROR) {
													
													logMsg = "Receive error! Thread: " + threadId;

													Log::WarningLog(logMsg);
													break;
												}

												if (bytesReceived == 0) {
													
													logMsg = "Cliente Desconectado! Thread: " + threadId;

													Log::InfoLog(logMsg);
													break;
												}

												if (bytesReceived > 0)
												{
													if (strcmp(revMsg, "100 OK") == 0) {

														value = teatro.getVisitas();

														str.assign(std::to_string(value)); // Converter inteiro para string

														sendData(client, str);

														ZeroMemory(revMsg, 1024);
														bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

														if (bytesReceived == SOCKET_ERROR) {
															
															logMsg = "Receive error! Thread: " + threadId;

															Log::WarningLog(logMsg);
															break;
														}

														if (bytesReceived == 0) {
															
															logMsg = "Cliente Desconectado! Thread: " + threadId;

															Log::InfoLog(logMsg);
															break;
														}

														if (bytesReceived > 0)
														{
															if (strcmp(revMsg, "100 OK") == 0) {



															}
														}
													}
													else {
														
														logMsg = "Erro de protocolo de comunicaçâo! Thread: " + threadId;

														Log::InfoLog(logMsg);
													}
												}
											}
											else {
												
												logMsg = "Erro de protocolo de comunicaçâo! Thread: " + threadId;

												Log::InfoLog(logMsg);
											}
										}
									}
									else {
										
										logMsg = "Erro de protocolo de comunicaçâo! Thread: " + threadId;

										Log::InfoLog(logMsg);
									}
								}
							}
							else {
								
								logMsg = "Erro de protocolo de comunicaçâo! Thread: " + threadId;

								Log::InfoLog(logMsg);
							}
						}
					}
					/*-------Loop------*/
				}

				logMsg = "Teatros enviados. Thread: " + threadId;

				Log::InfoLog(logMsg);

				sendData(client, "END");

				// À espera de uma resposta do cliente a dizer que recebeu os teatros
				ZeroMemory(revMsg, 1024);
				bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

				if (bytesReceived == SOCKET_ERROR) {
					
					logMsg = "Receive error! Thread: " + threadId;

					Log::WarningLog(logMsg);
					break;
				}

				if (bytesReceived == 0) {

					logMsg = "Cliente Desconectado! Thread: " + threadId;

					Log::InfoLog(logMsg);
					break;
				}

				if (bytesReceived > 0)
				{
					if (strcmp(revMsg, "100 OK") == 0) {

						sendData(client, "100 OK");
					
						// Eperar a resposta do id do teatro do cliente
						ZeroMemory(revMsg, 1024);
						bytesReceived = recv(client->getSocket(), revMsg, 1024, 0);

						if (bytesReceived == SOCKET_ERROR) {
							
							logMsg = "Receive error! Thread: " + threadId;

							Log::WarningLog(logMsg);
							break;
						}

						if (bytesReceived == 0) {
							
							logMsg = "Cliente Desconectado! Thread: " + threadId;

							Log::InfoLog(logMsg);
							break;
						}

						if (bytesReceived > 0)
						{
							int id = 0;

							id = atoi(revMsg); // Converter string para inteiro

							// Se a resposta for -1 então significa que o cliente quis voltar para o menu principal
							// Senão atualiza as visitas do respetivo teatro
							if (id != -1)
							{

								auxId = client->contains(client->getTeatrosVisitados(), id); // Verifica se o cliente já visitou o teatro

								// Se auxId for 0 não visitou o teatro
								// Senão visitou o teatro
								if (!auxId) {

									// Verificar se o id que foi enviado está na estrutura de dados
									// Se estiver continua senão manda um erro a dizer que não existe o teatro enviado
									for (auto &i : Teatro::getTeatros())
									{
										if (i.getId() == id)
										{
											isInTeatro = 1;
											break;
										}
									}

									// Se o teatro estiver na estrutura então adiciona nos teatros visitados e incrementa as visitas no ficheiro
									if (isInTeatro)
									{

										client->getTeatrosVisitados().push_back(id);

										logMsg = "Escolha recebida com sucesso. Thread: " + threadId;

										Log::InfoLog(logMsg);

										updateVisitas(Teatro::getTeatros(), id); // Atualiza o ficheiro teatros.csv
										client->UpdateClient();
									}
								}
								else
									isVisitado = 1; // Marcar que o teatro já foi visitado
							}
							else
								isSair = 1; // Marcar que o cliente quer voltar ao menu principal
						}
					}
				}

				logMsg = "A limpar estrutura de teatros. Thread: " + threadId;

				Log::InfoLog(logMsg);

				// Limpar a estrutura de teatros
				Teatro::getTeatros().clear();
				Teatro::getTeatros().shrink_to_fit();

				logMsg = "Estrutura de teatros limpa com sucesso. Thread: " + threadId;

				Log::InfoLog(logMsg);

				// Voltar para o menu principal
				if (isSair)
				{
					sendData(client, "300 LEFT");

					logMsg = "Pedido de compra cancelado. Thread: " + threadId;

					Log::InfoLog(logMsg);
				}
				// Teatro visitado ou não encontrado
				else if(isVisitado || !isInTeatro)
				{
					sendData(client, "404 NOT FOUND");

					logMsg = "Pedido de compra invalido. Thread: " + threadId;

					Log::InfoLog(logMsg);
				}
				// Incrementação das visitas ao respetivo teatro
				else
				{
					sendData(client, "100 OK");

					logMsg = "Pedido de compra finalizado. Thread: " + threadId;

					Log::InfoLog(logMsg);
				}
			}

			// Se o cliente selecionar a opção sair
			if (strcmp(revMsg, "400 BYE") == 0) {

				sendData(client, "400 BYE");

				logMsg = "Cliente Desconectado! Thread: " + threadId;

				Log::InfoLog(logMsg);

				break;
			}
		}
		
	}

	// Close the socket
	closesocket(client->getSocket());
}
