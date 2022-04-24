/* 
Simple winsock Server
*/

#include "server.h"

int sendData(Client* client, char* data) {
	char sendMsg[1024];
	char logMsg[150];

	int ws_result = 0;

	ZeroMemory(sendMsg, 1024);
	strcpy(sendMsg, data);

	ws_result = send(client->socket, sendMsg, strlen(sendMsg) + 1, 0); // Hanlde errors
	if (ws_result < 0)
	{
		memset(logMsg, '\0', 150);

		snprintf(logMsg, 150, "Connect Error! Thread: %d", client->threadID);

		WarningLog(logMsg);
		return 1;
	}

	return ws_result;
}

void updateVisitas(Teatro **teatros, int id, Client *client) {
	
	FILE* f;
	char logMsg[150];
	memset(logMsg, '\0', 150);

	f = fopen("teatros.csv", "w");

	memset(logMsg, '\0', 150);

	snprintf(logMsg, 150, "A incrementar o numero de visitados. Thread: %d", client->threadID);

	InfoLog(logMsg);

	if (f != NULL)
	{

		int numVis = 0;

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

		fclose(f);
	}
}

int compareTeatrosVisitados(const void* a, const void* b)
{
	return (*(int*)a - *(int*)b);
}

unsigned __stdcall ClientSession(Client* client) {
	char sendMsg[1024];
	char revMsg[1024];
	char logMsg[150];

	int bytesReceived = 0;
	int ws_result = 0;

	memset(logMsg, '\0', 150);

	snprintf(logMsg, 150, "User connected. Thread: %d", client->threadID);

	InfoLog(logMsg);

	while (TRUE)
	{
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
			//printf("%s\n", revMsg);
			if (strcmp(revMsg, "TEATRO") == 0) {

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "Pedido teatro. Thread: %d", client->threadID);

				InfoLog(logMsg);

				//mandar lista de teatros
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

				ZeroMemory(sendMsg, 1024);

				strcpy(sendMsg, "100 OK");

				sendData(client, sendMsg);

				// Ler ficheiro com os teatros
				DWORD resultMutexCidades = WaitForSingleObject(
					MutexCidades,    // handle to mutex
					INFINITE);  // no time-out interval

				// Ler ficheiro com os teatros
				DWORD resultMutexTeatros = WaitForSingleObject(
					MutexTeatros,    // handle to mutex
					INFINITE);  // no time-out interval

				float lat = 0.0f;
				float longi = 0.0f;

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "A obter a localizacao. Thread: %d", client->threadID);

				InfoLog(logMsg);

				int index = binarySearchCidade(cidades, 0, tamCidades, client->location);
				
				lat = getLat(cidades[index]);
				longi = getLong(cidades[index]);

				teatros = InitTeatros(lat, longi, client);

				int size = 0;
				int value = 0;
				char str[1024];

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "A enviar teatros.... Thread: %d", client->threadID);

				InfoLog(logMsg);

				for (size_t i = 0; i < tamTeatros; i++)
				{
					/*-------Loop------*/

					value = getId(teatros[i]);
					size = (int)((ceil(log10(value)) + 1) * sizeof(char));

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
									size = (int)((ceil(log10(value)) + 1) * sizeof(char));

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

													//free(str);

													value = getVisitas(teatros[i]);
													size = (int)((ceil(log10(value)) + 1) * sizeof(char));

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

				if (!ReleaseMutex(MutexCidades) && !ReleaseMutex(MutexTeatros))
				{
					// Handle error.
				}
				
			}

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

				ZeroMemory(sendMsg, 1024);

				strcpy(sendMsg, "100 OK");

				sendData(client, sendMsg);

				// Ler ficheiro com os teatros
				DWORD resultMutexCidades = WaitForSingleObject(
					MutexCidades,    // handle to mutex
					INFINITE);  // no time-out interval

				// Ler ficheiro com os teatros
				DWORD resultMutexTeatros = WaitForSingleObject(
					MutexTeatros,    // handle to mutex
					INFINITE);  // no time-out interval

				float lat = 0.0f;
				float longi = 0.0f;

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "A obter a localizacao. Thread: %d", client->threadID);

				InfoLog(logMsg);

				int index = binarySearchCidade(cidades, 0, tamCidades, client->location);

				lat = getLat(cidades[index]);
				longi = getLong(cidades[index]);

				teatros = InitTeatros(lat, longi, client);

				int size = 0;
				int value = 0;
				char str[1024];

				memset(logMsg, '\0', 150);

				snprintf(logMsg, 150, "A enviar teatros.... Thread: %d", client->threadID);

				InfoLog(logMsg);

				int auxId = -1;
				int isSair = 0;
				int isVisitado = 0;
				int isInTeatro = 0;

				for (size_t i = 0; i < tamTeatros; i++)
				{
					/*-------Loop------*/

					auxId = isTeatroVisitado(getId(teatros[i]), client);

					if (auxId != -1)
					{
						value = auxId;
						size = (int)((ceil(log10(value)) + 1) * sizeof(char));

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
										size = (int)((ceil(log10(value)) + 1) * sizeof(char));

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
														size = (int)((ceil(log10(value)) + 1) * sizeof(char));

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

							id = atoi(revMsg);

							if (id != -1)
							{
								auxId = isTeatroVisitado(id, client);

								if (auxId != -1) {
									for (size_t i = 0; i < tamTeatros; i++)
									{
										if (getId(teatros[i]) == auxId)
										{
											isInTeatro = 1;
											break;
										}
									}

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

										int visitasTeatro = -1;

										for (size_t i = 0; i < tamTeatros; i++)
										{
											if (getId(teatros[i]) == id)
											{
												visitasTeatro = getVisitas(teatros[i]);
											}
										}

										if (visitasTeatro != -1)
										{
											updateVisitas(teatros, id, client);
										}
										else
										{
											memset(logMsg, '\0', 150);

											snprintf(logMsg, 150, "Erro ao incrementar as visitas ao teatro com ID: %d. Thread: %d", id, client->threadID);

											WarningLog(logMsg);
										}
									}
								}
								else
									isVisitado = 1;
							}
							else
								isSair = 1;
						}
					}
				}

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

				if (isSair)
				{
					sendData(client, "300 LEFT");

					memset(logMsg, '\0', 150);

					snprintf(logMsg, 150, "Pedido de compra cancelado. Thread: %d", client->threadID);

					InfoLog(logMsg);
				}
				else if(isVisitado || !isInTeatro)
				{
					sendData(client, "404 NOT FOUND");

					memset(logMsg, '\0', 150);

					snprintf(logMsg, 150, "Pedido de compra invalido. Thread: %d", client->threadID);

					InfoLog(logMsg);
				}
				else
				{
					sendData(client, "100 OK");

					memset(logMsg, '\0', 150);

					snprintf(logMsg, 150, "Pedido de compra finalizado. Thread: %d", client->threadID);

					InfoLog(logMsg);
				}

				

				if (!ReleaseMutex(MutexCidades) && !ReleaseMutex(MutexTeatros))
				{
					// Handle error.
				}

			}

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

	//nao esquecer de fazer free de "client"
	//free(client);

	return 1;
}
