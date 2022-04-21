#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <math.h>

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable : 4996)


int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char message[1024], server_reply[2000];
	int ws_result;
	char local[50];
	char strRec[1024];
	int bytesReceived = 0;
	int cont = 1;
	int err = 0;
	int contador = 0;
	int comprar = 0;
	char  x;
	unsigned int tamT = 0;
	memset(local,'\0',50);

	// Initialise winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	
	//Create a socket
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	// create the socket  address (ip address and port)
	server.sin_addr.s_addr = inet_addr("25.58.1.108");
	server.sin_family = AF_INET;
	server.sin_port = htons(80);

	//Connect to remote server
	ws_result = connect(s, (struct sockaddr*)&server, sizeof(server));
	if (ws_result < 0)
	{
		puts("connect error");
		return 1;
	}
	int c;
	printf("Insira a sua Localizacao:\n->");
	scanf("%[a-zA-Z -]", local);
	while ((c = getchar()) != '\n' && c != EOF);
	system("cls");
	
	while (cont) {
		printf("\n");
		printf(" -----------\n");
		printf("| 1) Teatros |\n| 2) Comprar |\n| 0) Sair    |\n");
		printf(" ------------\n->");
		scanf("%c", &x);
		while ((c = getchar()) != '\n' && c != EOF);
		switch (x) {
		case '0':
			ZeroMemory(message, 1024);
			strcpy(message, "400 BYE");
			send(s, message, strlen(message), 0);
			cont = 0;
			break;
		case '1':
			system("cls");
			ZeroMemory(message, 1024);
			strcpy(message, "TEATRO");
			ws_result = send(s, message, strlen(message), 0);

			if (ws_result < 0)
			{
				return 1;
			}


			ZeroMemory(message, 1024);
			ZeroMemory(strRec, 1024);
			bytesReceived = recv(s, strRec, 1024, 0);
			if (bytesReceived == SOCKET_ERROR) {
				printf("\nReceive error!\n");
				cont = 0;
				break;
			}
			if (bytesReceived > 0) {
				if (strcmp(strRec, "100 OK") == 0) {


				}
				else {
					cont = 0;
				}
			}

			//esta variavel dps vai receber o valor 
			ws_result = send(s, local, strlen(local), 0);
			if (ws_result < 0)
			{
				cont = 0;
				break;
			}

			ZeroMemory(strRec, 1024);
			bytesReceived = recv(s, strRec, 1024, 0);
			if (bytesReceived == SOCKET_ERROR) {
				printf("\nReceive error!\n");
				cont = 0;
				break;
			}
			if (bytesReceived > 0) {
				if (strcmp(strRec, "100 OK") == 0) {
					//id 100ok nometeatro 100ok numespetaculos 100ok loc 100ok numvisitas 100ok
					// 0         1                    2                3        4
					printf("\n");
					ZeroMemory(strRec, 1024);
					bytesReceived = recv(s, strRec, 1024, 0);
					if (bytesReceived == SOCKET_ERROR) {
						printf("\nReceive error!\n");
						cont = 0;
						err = 1;
						break;
					}
					else if (bytesReceived == 0) {
						err = 1;


					}

					while ((strcmp(strRec, "END") != 0)) {
						if (contador < 4) {
							printf("%s | ", strRec);
							ZeroMemory(message, 1024);
							strcpy(message, "100 OK");
							send(s, message, strlen(message), 0);
							contador++;
							ZeroMemory(strRec, 1024);
							bytesReceived = recv(s, strRec, 1024, 0);

						}
						else if (contador == 4) {
							contador = 0;
							printf("%s\n", strRec);
							ZeroMemory(message, 1024);
							strcpy(message, "100 OK");
							send(s, message, strlen(message), 0);
							ZeroMemory(strRec, 1024);
							bytesReceived = recv(s, strRec, 1024, 0);

						}

					}

					ZeroMemory(message, 1024);
					strcpy(message, "100 OK");
					send(s, message, strlen(message), 0);
				}
				else //se nao mandar 100OK
				{
					cont = 0;
					break;

				}
			}
			break;

		case '2':
			system("cls");
			ZeroMemory(message, 1024);
			strcpy(message, "COMPRA");
			ws_result = send(s, message, strlen(message), 0);

			if (ws_result < 0)
			{
				puts("Send failed");
				return 1;
			}


			ZeroMemory(message, 1024);
			ZeroMemory(strRec, 1024);
			bytesReceived = recv(s, strRec, 1024, 0);
			if (bytesReceived == SOCKET_ERROR) {
				printf("\nReceive error!\n");
				cont = 0;
				break;
			}
			if (bytesReceived > 0) {
				if (strcmp(strRec, "100 OK") == 0) {
					//esta variavel dps vai receber o valor 
					ws_result = send(s, local, strlen(local), 0);
					if (ws_result < 0)
					{
						cont = 0;
						break;
					}
					ZeroMemory(message, 1024);
					ZeroMemory(strRec, 1024);
					bytesReceived = recv(s, strRec, 1024, 0);
					if (bytesReceived == SOCKET_ERROR) {
						printf("\nReceive error!\n");
						cont = 0;
						break;
					}
					if (bytesReceived > 0) {
						if (strcmp(strRec, "100 OK") == 0) {
							printf("\n");
							ZeroMemory(strRec, 1024);
							bytesReceived = recv(s, strRec, 1024, 0);
							if (bytesReceived == SOCKET_ERROR) {
								printf("\nReceive error!\n");
								cont = 0;
								err = 1;
								break;
							}
							else if (bytesReceived == 0) {
								err = 1;


							}

							while ((strcmp(strRec, "END") != 0)) {
								if (contador < 4) {
									printf("%s | ", strRec);
									ZeroMemory(message, 1024);
									strcpy(message, "100 OK");
									send(s, message, strlen(message), 0);
									contador++;
									ZeroMemory(strRec, 1024);
									bytesReceived = recv(s, strRec, 1024, 0);

								}
								else if (contador == 4) {
									contador = 0;
									printf("%s\n", strRec);
									ZeroMemory(message, 1024);
									strcpy(message, "100 OK");
									send(s, message, strlen(message), 0);
									ZeroMemory(strRec, 1024);
									bytesReceived = recv(s, strRec, 1024, 0);

								}

							}

							ZeroMemory(message, 1024);
							strcpy(message, "100 OK");
							send(s, message, strlen(message), 0);
							ZeroMemory(strRec, 1024);
							bytesReceived = recv(s, strRec, 1024, 0);

							if (bytesReceived == SOCKET_ERROR) {
								printf("\nReceive error!\n");
								cont = 0;
								err = 1;
								break;
							}
							else if (bytesReceived == 0) {
								err = 1;
							}

							if (bytesReceived > 0) {
								if (strcmp(strRec, "100 OK") == 0) {
									printf("-1 para Sair");
									printf("\nSelecione o numero do Teatro que ja visitou:\n->");
									scanf("%d", &comprar);
									while ((c = getchar()) != '\n' && c != EOF);

									char str[4];
									memset(str, '\0', 4);
									itoa(comprar, str, 10);
									send(s, str, strlen(str), 0);

									ZeroMemory(strRec, 1024);
									bytesReceived = recv(s, strRec, 1024, 0);

									if (bytesReceived == SOCKET_ERROR) {
										printf("\nReceive error!\n");
										cont = 0;
										err = 1;
										break;
									}
									else if (bytesReceived == 0) {
										err = 1;
									}

									if (bytesReceived > 0) {
										printf("\n%s\n", strRec);
										if (strcmp(strRec, "100 OK") == 0) {
											system("cls");
											printf("Compra efetuada com sucesso\n\n");
										}
										if (strcmp(strRec, "300 LEFT") == 0) {
											system("cls");
											printf("Pedido de compra cancelada\n\n");
										}

									}
								}
							}
						}
					}
				}
			}
		}
	}
	//Send some data
// Close the socket
	closesocket(s);
	//Cleanup winsock
	WSACleanup();
	return 0;

}