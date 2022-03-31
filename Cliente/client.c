#include "Teatro.h"
#include <winsock2.h>


#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable : 4996)


int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char message[1024], server_reply[2000];
	int ws_result;
	char local[50] = "Vila Real\0";
	char strRec[1024];
	int bytesReceived = 0;
	int cont = 1;
	int err = 0;
	char  x;
	Teatro** t = NULL;
	Teatro** auxT = NULL;
	unsigned int tamT = 0;


	// Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	//Create a socket
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");


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


	puts("100");
	int c;
	while (cont) {
		printf("\n1 Teatros\n2 Comprar\n0 Sair ->");
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
			ZeroMemory(message, 1024);
			strcpy(message, "TEATRO");
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


				}
				else {
					cont = 0;
				}
			}

			//esta variavel dps vai receber o valor 
			ws_result = send(s, local, strlen(local), 0);
			if (ws_result < 0)
			{
				puts("Send failed");
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

					puts("Reply received\n");
				}
				else
				{
					cont = 0;
					break;

				}
			}
			do {
				ZeroMemory(strRec, 1024);
				bytesReceived = recv(s, strRec, 1024, 0);
				if (bytesReceived == SOCKET_ERROR) {
					printf("\nReceive error!\n");
					cont = 0;
					break;
				}
				if (bytesReceived > 0) {
					puts(strRec);
					ZeroMemory(message, 1024);
					strcpy(message, "100 OK");
					send(s, message, strlen(message), 0);


				}
				if (bytesReceived == 0) {
					puts("ola!\n");


				}

			} while ((strcmp(strRec, "END") != 0));
			break;

		case '2':
			ZeroMemory(message, 1024);
			strcpy(message, "TEATRO");
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


				}
				else {
					cont = 0;
				}
			}

			//esta variavel dps vai receber o valor 
			ws_result = send(s, local, strlen(local), 0);
			if (ws_result < 0)
			{
				puts("Send failed");
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


				}
				else
				{
					cont = 0;
					break;

				}
			}


			ZeroMemory(strRec, 1024);
			bytesReceived = recv(s, strRec, 1024, 0);
			if (bytesReceived == SOCKET_ERROR) {
				printf("\nReceive error!\n");
				cont = 0;
				err = 1;
				break;
			}
			else if (bytesReceived == 0) {
				puts("ola!\n");
				err = 1;


			}

			unsigned int maxSTR = 0;

			while ((strcmp(strRec, "END") != 0)) {
				tamT++;
				auxT = (Teatro**)realloc(t, sizeof(Teatro*) * tamT);
				if (auxT == NULL) {
					printf("Ocorreu um erro");
					if (t != NULL) {
						int i;
						for (i = 0; i < tamT; i++) {
							if (t[i] != NULL) {
								free(t[i]->id);
								free(t[i]->nome);
								free(t[i]->localizacao);
								free(t[i]);
								err = 1;
							}
						}
					}
					break;
				}
				else t = auxT;
				unsigned int strTam = strlen(strRec);
				if (strTam > maxSTR)
					maxSTR = strTam;
				char* auxS = (char*)malloc(strTam);
				strcpy(auxS, strRec);
				t[tamT - 1] = Criar_Teatro(auxS, auxS, "vazio");
				printf("%s - %s, %s\n", getId(t[tamT - 1]), getNome(t[tamT - 1]), getLocalizacao(t[tamT - 1]));

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
					puts("ola!\n");
					err = 1;

				}
			}

			if (!err) {
				printf("Qual deseja comprar:");
				char* idselect = (char*)malloc(maxSTR);
				if (idselect != NULL) {
					scanf_s("%s", idselect, maxSTR);
					ZeroMemory(message, 1024);
					strcpy(message, "COMPRA");
					ws_result = send(s, message, strlen(message), 0);

					ZeroMemory(strRec, 1024);
					bytesReceived = recv(s, strRec, 1024, 0);
					if (bytesReceived == SOCKET_ERROR) {
						printf("\nReceive error!\n");
						cont = 0;
						break;
					}
					if (bytesReceived > 0) {
						if (strcmp(strRec, "100 OK") == 0) {
							ws_result = send(s, idselect, maxSTR, 0);
							ZeroMemory(strRec, 1024);
							bytesReceived = recv(s, strRec, 1024, 0);
							if (bytesReceived == SOCKET_ERROR) {
								printf("\nReceive error!\n");
								cont = 0;
								break;
							}
							if (bytesReceived > 0) {
								if (strcmp(strRec, "100 OK") == 0) {
									puts("comprado");
								}
								else if (strcmp(strRec, "404 BAD") == 0) {
									puts("erro na compra");
								}
							}
						}
						else
						{
							cont = 0;
							break;

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