#include "server.h"

/**
* Função para mandar dados para um cliente
* @param Client* client
* @param char** data
* @return int - retorna o restulado do envio dos dados
*/
int sendData(Client* client, char* data) {
	char sendMsg[1024];
	char logMsg[150];

	int ws_result = 0;

	ZeroMemory(sendMsg, 1024);
	strcpy(sendMsg, data);

	ws_result = send(client->socket, sendMsg, strlen(sendMsg) + 1, 0); // Função para enviar dados para o cliente

	// Hanlde errors
	if (ws_result < 0)
	{
		memset(logMsg, '\0', 150); // Limpar a variável

		snprintf(logMsg, 150, "Connect Error! Thread: %d", client->threadID); // Copiar a string para a variável

		WarningLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
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
void updateVisitas(Teatro **teatros, int id, Client *client) {
	
	FILE* f;
	char logMsg[150];
	memset(logMsg, '\0', 150); // Limpar a variável

	f = fopen("teatros.csv", "w"); // Abrir o ficheiro teatros.csv

	memset(logMsg, '\0', 150); // Limpar a variável

	snprintf(logMsg, 150, "A incrementar o numero de visitados. Thread: %d", client->threadID); // Copiar a string para a variável

	InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal

	// Verificar se abriu o ficheiro corretamente
	if (f != NULL)
	{
		for (size_t i = 0; i < tamTeatros; i++)
		{
			if (id == getId(teatros[i]))
			{
				fprintf(f, "%d,%s,%d,%s,%d\n", getId(teatros[i]), getTeatroNome(teatros[i]), getEspetaculos(teatros[i]), getLocalizacao(teatros[i]), (getVisitas(teatros[i]) + 1));
			}
			else
			{
				fprintf(f, "%d,%s,%d,%s,%d\n", getId(teatros[i]), getTeatroNome(teatros[i]), getEspetaculos(teatros[i]), getLocalizacao(teatros[i]), getVisitas(teatros[i]));
			}
		}

		fclose(f); // Fechar o ficheiro
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
unsigned __stdcall ClientSession(Client* client) {
	char sendMsg[1024];
	char revMsg[1024];
	char logMsg[150];

	int bytesReceived = 0;
	int ws_result = 0;

	memset(logMsg, '\0', 150); // Limpar a variável

	snprintf(logMsg, 150, "User connected. Thread: %d", client->threadID); // Copiar a string para a variável

	InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal

	// Loop da thread para ficar atento aos pedidos do cliente
	while (TRUE)
	{
		ZeroMemory(revMsg, 1024); // Limpar a variavel

		bytesReceived = recv(client->socket, revMsg, 1024, 0); // Ficar à espera de uma resposta do cliente

		// Erro inesperado do cliente
		if (bytesReceived == SOCKET_ERROR) {
			memset(logMsg, '\0', 150); // Limpar a variável

			snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID); // Copiar a string para a variável

			WarningLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
			break;
		}

		// Cliente desconectado
		if (bytesReceived == 0) {
			memset(logMsg, '\0', 150); // Limpar a variável

			snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID); // Copiar a string para a variável

			InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
			break;
		}

		// A thread recebeu informação sem erro
		if (bytesReceived > 0)
		{
			//Verificar qual o pedido do cliente

			// Se fizer um pedido de TEATRO
			// Iremos mandar os teatros ao cliente
			if (strcmp(revMsg, "TEATRO") == 0) {

				memset(logMsg, '\0', 150); // Limpar a variável

				snprintf(logMsg, 150, "Pedido teatro. Thread: %d", client->threadID); // Copiar a string para a variável

				InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal

				ZeroMemory(revMsg, 1024); // Limpar a variável
				ZeroMemory(sendMsg, 1024); // Limpar a variável

				strcpy(sendMsg, "100 OK"); // Copiar a string para a variável

				sendData(client, sendMsg); // Mandar dados para o cliente

				bytesReceived = recv(client->socket, revMsg, 1024, 0);

				if (bytesReceived == SOCKET_ERROR) {
					memset(logMsg, '\0', 150); // Limpar a variável

					snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID); // Copiar a string para a variável

					WarningLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
					break;
				}

				if (bytesReceived == 0) {
					memset(logMsg, '\0', 150); // Limpar a variável

					snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID); // Copiar a string para a variável

					InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
					break;
				}

				strcpy(client->location, revMsg); // Copiar a string para a variável

				// Bloquear o mutex das cidades
				DWORD resultMutexCidades = WaitForSingleObject(
					MutexCidades,    // handle to mutex
					INFINITE);  // no time-out interval

				// Bloquear o mutex dos teatros
				DWORD resultMutexTeatros = WaitForSingleObject(
					MutexTeatros,    // handle to mutex
					INFINITE);  // no time-out interval

				ZeroMemory(sendMsg, 1024); // Limpar a variável

				strcpy(sendMsg, "100 OK"); // Copiar a string para a variável

				sendData(client, sendMsg); // Mandar dados para o cliente

				float lat = 0.0f;
				float longi = 0.0f;

				memset(logMsg, '\0', 150); // Limpar a variável

				snprintf(logMsg, 150, "A obter a localizacao. Thread: %d", client->threadID); // Copiar a string para a variável

				InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal

				int index = binarySearchCidade(cidades, 0, tamCidades, client->location); // obter o index da cidade do cliente
				
				lat = getLat(cidades[index]); // Obter a latitude da cidade
				longi = getLong(cidades[index]); // Obter a longitude da cidade

				teatros = InitTeatros(lat, longi, client); // Ler os teatros do ficheiro e guardar numa estrutura

				int value = 0;
				char str[1024];

				memset(logMsg, '\0', 150); // Limpar a variável

				snprintf(logMsg, 150, "A enviar teatros.... Thread: %d", client->threadID); // Copiar a string para a variável

				InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal

				// Enviar os teatros para o cliente
				for (size_t i = 0; i < tamTeatros; i++)
				{
					/*-------Loop------*/

					value = getId(teatros[i]); // Obter o id do teatro

					memset(str, '\0', 1024); // Limpar a variável

					itoa(value, str, 10); // Converter inteiro para string

					sendData(client, str); // Mandar dados para o cliente

					ZeroMemory(revMsg, 1024);
					bytesReceived = recv(client->socket, revMsg, 1024, 0);

					if (bytesReceived == SOCKET_ERROR) {
						memset(logMsg, '\0', 150);

						snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID);

						WarningLog(logMsg);
						break;
					}

					if (bytesReceived == 0) {
						memset(logMsg, '\0', 150);

						snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

						InfoLog(logMsg);
						break;
					}

					if (bytesReceived > 0)
					{
						if (strcmp(revMsg, "100 OK") == 0) {

							sendData(client, getTeatroNome(teatros[i]));

							ZeroMemory(revMsg, 1024);
							bytesReceived = recv(client->socket, revMsg, 1024, 0);

							if (bytesReceived == SOCKET_ERROR) {
								memset(logMsg, '\0', 150);

								snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID);

								WarningLog(logMsg);
								break;
							}

							if (bytesReceived == 0) {
								memset(logMsg, '\0', 150);

								snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

								InfoLog(logMsg);
								break;
							}

							if (bytesReceived > 0)
							{
								if (strcmp(revMsg, "100 OK") == 0) {

									value = getEspetaculos(teatros[i]);

									memset(str, '\0', 1024);

									itoa(value, str, 10);

									sendData(client, str);

									ZeroMemory(revMsg, 1024);
									bytesReceived = recv(client->socket, revMsg, 1024, 0);

									if (bytesReceived == SOCKET_ERROR) {
										memset(logMsg, '\0', 150);

										snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID);

										WarningLog(logMsg);
										break;
									}

									if (bytesReceived == 0) {
										memset(logMsg, '\0', 150);

										snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

										InfoLog(logMsg);
										break;
									}

									if (bytesReceived > 0)
									{
										if (strcmp(revMsg, "100 OK") == 0) {

											sendData(client, getLocalizacao(teatros[i]));

											ZeroMemory(revMsg, 1024);
											bytesReceived = recv(client->socket, revMsg, 1024, 0);

											if (bytesReceived == SOCKET_ERROR) {
												memset(logMsg, '\0', 150);

												snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID);

												WarningLog(logMsg);
												break;
											}

											if (bytesReceived == 0) {
												memset(logMsg, '\0', 150);

												snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

												InfoLog(logMsg);
												break;
											}

											if (bytesReceived > 0)
											{
												if (strcmp(revMsg, "100 OK") == 0) {

													value = getVisitas(teatros[i]);

													memset(str, '\0', 1024);

													itoa(value, str, 10);

													sendData(client, str);

													ZeroMemory(revMsg, 1024);
													bytesReceived = recv(client->socket, revMsg, 1024, 0);

													if (bytesReceived == SOCKET_ERROR) {
														memset(logMsg, '\0', 150);

														snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID);

														WarningLog(logMsg);
														break;
													}

													if (bytesReceived == 0) {
														memset(logMsg, '\0', 150);

														snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

														InfoLog(logMsg);
														break;
													}

													if (bytesReceived > 0)
													{
														if (strcmp(revMsg, "100 OK") == 0) {

															
														}
													}
												}
												else {
													memset(logMsg, '\0', 150);

													snprintf(logMsg, 150, "Erro de protocolo de comunicaçâo! Thread: %d", client->threadID);

													InfoLog(logMsg);
												}
											}
										}
										else {
											memset(logMsg, '\0', 150);

											snprintf(logMsg, 150, "Erro de protocolo de comunicaçâo! Thread: %d", client->threadID);

											InfoLog(logMsg);
										}
									}
								}
								else {
									memset(logMsg, '\0', 150);

									snprintf(logMsg, 150, "Erro de protocolo de comunicaçâo! Thread: %d", client->threadID);

									InfoLog(logMsg);
								}
							}
						}
						else {
							memset(logMsg, '\0', 150);

							snprintf(logMsg, 150, "Erro de protocolo de comunicaçâo! Thread: %d", client->threadID);

							InfoLog(logMsg);
						}
					}

					/*-------Loop------*/
				}

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "Teatros enviados. Thread: %d", client->threadID);

				InfoLog(logMsg);

				// Limpar a estrutura de teatros

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "A limpar estrutura de teatros. Thread: %d", client->threadID);

				InfoLog(logMsg);

				for (size_t i = 0; i < tamTeatros; i++)
				{
					free(teatros[i]);
				}
				free(teatros);

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "Estrutura de teatros limpa com sucesso. Thread: %d", client->threadID);

				InfoLog(logMsg);

				sendData(client, "END");

				ZeroMemory(revMsg, 1024);
				bytesReceived = recv(client->socket, revMsg, 1024, 0);

				if (bytesReceived == SOCKET_ERROR) {
					memset(logMsg, '\0', 150);

					snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID);

					WarningLog(logMsg);
					break;
				}

				if (bytesReceived == 0) {
					memset(logMsg, '\0', 150);

					snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

					InfoLog(logMsg);
					break;
				}

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "Pedido teatro finalizado. Thread: %d", client->threadID);

				InfoLog(logMsg);

				// Libertar os mutexes
				if (!ReleaseMutex(MutexCidades))
				{
					// Handle error.
				}

				if (!ReleaseMutex(MutexTeatros))
				{
					// Handle error.
				}
				
			}

			// Se fizer um pedido de COMPRA
			// Iremos mandar os teatros não visitados pelo cliente
			// e esperar por uma resposta do cliente de qual é que
			// teatro já visitou
			if (strcmp(revMsg, "COMPRA") == 0) {

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "Pedido compra. Thread: %d", client->threadID);

				InfoLog(logMsg);

				ZeroMemory(revMsg, 1024);
				ZeroMemory(sendMsg, 1024);

				strcpy(sendMsg, "100 OK");

				sendData(client, sendMsg);

				bytesReceived = recv(client->socket, revMsg, 1024, 0);

				if (bytesReceived == SOCKET_ERROR) {
					memset(logMsg, '\0', 150);

					snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID);

					WarningLog(logMsg);
					break;
				}

				if (bytesReceived == 0) {
					memset(logMsg, '\0', 150);

					snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

					InfoLog(logMsg);
					break;
				}

				strcpy(client->location, revMsg);

				// Bloquear o mutex das cidades
				DWORD resultMutexCidades = WaitForSingleObject(
					MutexCidades,    // handle to mutex
					INFINITE);  // no time-out interval

				// Bloquear o mutex dos teatros
				DWORD resultMutexTeatros = WaitForSingleObject(
					MutexTeatros,    // handle to mutex
					INFINITE);  // no time-out interval

				ZeroMemory(sendMsg, 1024);

				strcpy(sendMsg, "100 OK");

				sendData(client, sendMsg);

				float lat = 0.0f;
				float longi = 0.0f;

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "A obter a localizacao. Thread: %d", client->threadID);

				InfoLog(logMsg);

				int index = binarySearchCidade(cidades, 0, tamCidades, client->location); // obter o index da cidade do cliente

				lat = getLat(cidades[index]); // Obter a latitude da cidade
				longi = getLong(cidades[index]); // Obter a longitude da cidade

				teatros = InitTeatros(lat, longi, client); // Ler os teatros do ficheiro e guardar numa estrutura
				int value = 0;
				char str[1024];

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "A enviar teatros.... Thread: %d", client->threadID);

				InfoLog(logMsg);

				int auxId = -1;
				int isSair = 0;
				int isVisitado = 0;
				int isInTeatro = 0;

				// Enviar os teatros para o cliente
				for (size_t i = 0; i < tamTeatros; i++)
				{
					/*-------Loop------*/

					auxId = isTeatroVisitado(getId(teatros[i]), client);

					if (auxId != -1)
					{
						value = auxId;

						memset(str, '\0', 1024);

						itoa(value, str, 10);

						sendData(client, str);

						ZeroMemory(revMsg, 1024);
						bytesReceived = recv(client->socket, revMsg, 1024, 0);

						if (bytesReceived == SOCKET_ERROR) {
							memset(logMsg, '\0', 150);

							snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID);

							WarningLog(logMsg);
							break;
						}

						if (bytesReceived == 0) {
							memset(logMsg, '\0', 150);

							snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

							InfoLog(logMsg);
							break;
						}

						if (bytesReceived > 0)
						{
							if (strcmp(revMsg, "100 OK") == 0) {

								sendData(client, getTeatroNome(teatros[i]));

								ZeroMemory(revMsg, 1024);
								bytesReceived = recv(client->socket, revMsg, 1024, 0);

								if (bytesReceived == SOCKET_ERROR) {
									memset(logMsg, '\0', 150);

									snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID);

									WarningLog(logMsg);
									break;
								}

								if (bytesReceived == 0) {
									memset(logMsg, '\0', 150);

									snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

									InfoLog(logMsg);
									break;
								}

								if (bytesReceived > 0)
								{
									if (strcmp(revMsg, "100 OK") == 0) {

										//free(str);

										value = getEspetaculos(teatros[i]);

										memset(str, '\0', 1024);

										itoa(value, str, 10);

										sendData(client, str);

										ZeroMemory(revMsg, 1024);
										bytesReceived = recv(client->socket, revMsg, 1024, 0);

										if (bytesReceived == SOCKET_ERROR) {
											memset(logMsg, '\0', 150);

											snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID);

											WarningLog(logMsg);
											break;
										}

										if (bytesReceived == 0) {
											memset(logMsg, '\0', 150);

											snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

											InfoLog(logMsg);
											break;
										}

										if (bytesReceived > 0)
										{
											if (strcmp(revMsg, "100 OK") == 0) {

												sendData(client, getLocalizacao(teatros[i]));

												ZeroMemory(revMsg, 1024);
												bytesReceived = recv(client->socket, revMsg, 1024, 0);

												if (bytesReceived == SOCKET_ERROR) {
													memset(logMsg, '\0', 150);

													snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID);

													WarningLog(logMsg);
													break;
												}

												if (bytesReceived == 0) {
													memset(logMsg, '\0', 150);

													snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

													InfoLog(logMsg);
													break;
												}

												if (bytesReceived > 0)
												{
													if (strcmp(revMsg, "100 OK") == 0) {

														value = getVisitas(teatros[i]);

														memset(str, '\0', 1024);

														itoa(value, str, 10);

														sendData(client, str);

														ZeroMemory(revMsg, 1024);
														bytesReceived = recv(client->socket, revMsg, 1024, 0);

														if (bytesReceived == SOCKET_ERROR) {
															memset(logMsg, '\0', 150);

															snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID);

															WarningLog(logMsg);
															break;
														}

														if (bytesReceived == 0) {
															memset(logMsg, '\0', 150);

															snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

															InfoLog(logMsg);
															break;
														}

														if (bytesReceived > 0)
														{
															if (strcmp(revMsg, "100 OK") == 0) {



															}
														}
													}
													else {
														memset(logMsg, '\0', 150);

														snprintf(logMsg, 150, "Erro de protocolo de comunicaçâo! Thread: %d", client->threadID);

														InfoLog(logMsg);
													}
												}
											}
											else {
												memset(logMsg, '\0', 150);

												snprintf(logMsg, 150, "Erro de protocolo de comunicaçâo! Thread: %d", client->threadID);

												InfoLog(logMsg);
											}
										}
									}
									else {
										memset(logMsg, '\0', 150);

										snprintf(logMsg, 150, "Erro de protocolo de comunicaçâo! Thread: %d", client->threadID);

										InfoLog(logMsg);
									}
								}
							}
							else {
								memset(logMsg, '\0', 150);

								snprintf(logMsg, 150, "Erro de protocolo de comunicaçâo! Thread: %d", client->threadID);

								InfoLog(logMsg);
							}
						}
					}
					/*-------Loop------*/
				}

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "Teatros enviados. Thread: %d", client->threadID);

				InfoLog(logMsg);

				sendData(client, "END");

				// À espera de uma resposta do cliente a dizer que recebeu os teatros
				ZeroMemory(revMsg, 1024);
				bytesReceived = recv(client->socket, revMsg, 1024, 0);

				if (bytesReceived == SOCKET_ERROR) {
					memset(logMsg, '\0', 150);

					snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID);

					WarningLog(logMsg);
					break;
				}

				if (bytesReceived == 0) {
					memset(logMsg, '\0', 150);

					snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

					InfoLog(logMsg);
					break;
				}

				if (bytesReceived > 0)
				{
					if (strcmp(revMsg, "100 OK") == 0) {

						sendData(client, "100 OK");
					
						// Eperar a resposta do id do teatro do cliente
						ZeroMemory(revMsg, 1024);
						bytesReceived = recv(client->socket, revMsg, 1024, 0);

						if (bytesReceived == SOCKET_ERROR) {
							memset(logMsg, '\0', 150);

							snprintf(logMsg, 150, "Receive error! Thread: %d", client->threadID);

							WarningLog(logMsg);
							break;
						}

						if (bytesReceived == 0) {
							memset(logMsg, '\0', 150);

							snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

							InfoLog(logMsg);
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
								auxId = isTeatroVisitado(id, client); // Verifica se o cliente já visitou o teatro

								// Se auxId for -1 não visitou o teatro
								// Senão visitou o teatro
								if (auxId != -1) {

									// Verificar se o id que foi enviado está na estrutura de dados
									// Se estiver continua senão manda um erro a dizer que não existe o teatro enviado
									for (size_t i = 0; i < tamTeatros; i++)
									{
										if (getId(teatros[i]) == auxId)
										{
											isInTeatro = 1;
											break;
										}
									}

									// Se o teatro estiver na estrutura então adiciona nos teatros visitados e incrementa as visitas no ficheiro
									if (isInTeatro)
									{
										if (client->tamTeatrosVisitados == 0)
										{
											client->tamTeatrosVisitados++;

											client->teatroVisitados = calloc(1, sizeof(int));

											if (client->teatroVisitados != NULL) {
												*(client->teatroVisitados) = id;
												memset(logMsg, '\0', 150);

												snprintf(logMsg, 150, "Escolha recebida com sucesso. Thread: %d", client->threadID);

												InfoLog(logMsg);
											}
											else {
												client->tamTeatrosVisitados--;

												memset(logMsg, '\0', 150);

												snprintf(logMsg, 150, "Erro ao receber escolha. Thread: %d", client->threadID);

												WarningLog(logMsg);
											}

										}
										else {

											int* auxTeatrosVisitados = NULL;

											client->tamTeatrosVisitados++;

											auxTeatrosVisitados = calloc(client->tamTeatrosVisitados, sizeof(int));

											if (auxTeatrosVisitados != NULL)
											{
												int size = (client->tamTeatrosVisitados - 1);

												memmove(auxTeatrosVisitados, client->teatroVisitados, sizeof(int) * size);

												*(auxTeatrosVisitados + (client->tamTeatrosVisitados - 1)) = id;

												free(client->teatroVisitados);

												client->teatroVisitados = auxTeatrosVisitados;

												auxTeatrosVisitados = NULL;

												qsort(client->teatroVisitados, client->tamTeatrosVisitados, sizeof(int), compareTeatrosVisitados);

												memset(logMsg, '\0', 150);

												snprintf(logMsg, 150, "Escolha recebida com sucesso. Thread: %d", client->threadID);

												InfoLog(logMsg);

											}
											else {
												client->tamTeatrosVisitados--;

												memset(logMsg, '\0', 150);

												snprintf(logMsg, 150, "Erro ao receber escolha. Thread: %d", client->threadID);

												WarningLog(logMsg);
											}
										}

										updateVisitas(teatros, id, client); // Atualiza o ficheiro teatros.csv
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

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "A limpar estrutura de teatros. Thread: %d", client->threadID);

				InfoLog(logMsg);


				// Limpar a estrutura de teatros
				for (size_t i = 0; i < tamTeatros; i++)
				{
					free(teatros[i]);
				}
				free(teatros);

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "Estrutura de teatros limpa com sucesso. Thread: %d", client->threadID);

				InfoLog(logMsg);

				// Voltar para o menu principal
				if (isSair)
				{
					sendData(client, "300 LEFT");

					memset(logMsg, '\0', 150);

					snprintf(logMsg, 150, "Pedido de compra cancelado. Thread: %d", client->threadID);

					InfoLog(logMsg);
				}
				// Teatro visitado ou não encontrado
				else if(isVisitado || !isInTeatro)
				{
					sendData(client, "404 NOT FOUND");

					memset(logMsg, '\0', 150);

					snprintf(logMsg, 150, "Pedido de compra invalido. Thread: %d", client->threadID);

					InfoLog(logMsg);
				}
				// Incrementação das visitas ao respetivo teatro
				else
				{
					sendData(client, "100 OK");

					memset(logMsg, '\0', 150);

					snprintf(logMsg, 150, "Pedido de compra finalizado. Thread: %d", client->threadID);

					InfoLog(logMsg);
				}

				// Libertar os mutexes
				if (!ReleaseMutex(MutexCidades))
				{
					// Handle error.
				}

				if (!ReleaseMutex(MutexTeatros))
				{
					// Handle error.
				}

			}

			// Se o cliente selecionar a opção sair
			if (strcmp(revMsg, "400 BYE") == 0) {

				ZeroMemory(sendMsg, 1024);

				strcpy(sendMsg, "400 BYE");
				send(client->socket, sendMsg, strlen(sendMsg) + 1, 0); // Hanlde errors

				
				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "Cliente Desconectado! Thread: %d", client->threadID);

				InfoLog(logMsg);

				break;
			}
		}
		
	}

	// Close the socket
	closesocket(client->socket);

	return 1;
}
