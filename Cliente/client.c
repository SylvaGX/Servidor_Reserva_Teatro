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
	
	int c;
	
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
//	server.sin_addr.s_addr = inet_addr("25.58.1.108");
	server.sin_addr.s_addr = inet_addr("148.63.161.37");
	server.sin_family = AF_INET;
	//server.sin_port = htons(80); //porta 80
	server.sin_port = htons(45370); 
	
	//Connect to remote server
	ws_result = connect(s, (struct sockaddr*)&server, sizeof(server));
	if (ws_result < 0)
	{
		puts("connect error");
		return 1;
	}

	ZeroMemory(strRec, 1024);
	bytesReceived = recv(s, strRec, 1024, 0); //receive 
	if (bytesReceived == SOCKET_ERROR) {
		printf("\nReceive error!\n");
		cont = 0;
	}
	if (bytesReceived > 0) {
		if (strcmp(strRec, "500 LOC") == 0) { //caso de ter recebido a confirmacao 
			printf("Insira a sua Localizacao(cidades e concelhos de Portugal):\n->");
			scanf("%[a-zA-Z -]", local);//expressão regular para todos os caracteres de a a z, Espaço e - 
			while ((c = getchar()) != '\n' && c != EOF); //clear "Enters"
			system("cls");

			//Envia a localizacao que a pessoa escreveu
			ws_result = send(s, local, strlen(local), 0);
			if (ws_result < 0)//caso de erro (nao enviou localizacao)
			{
				cont = 0;

			}
			int confloc = 1;
			while (confloc){
				ZeroMemory(strRec, 1024);
				bytesReceived = recv(s, strRec, 1024, 0); //receive 
				if (bytesReceived == SOCKET_ERROR) {
					printf("\nReceive error!\n");
					cont = 0;
				}

				if (bytesReceived > 0) {
					if (strcmp(strRec, "100 OK") == 0) {//caso da cidade estar no ficheiro
						confloc = 0;
					}
					if (strcmp(strRec, "404 NOT FOUND") == 0) {//caso da cidade não estar no ficheiro
						printf("Insira a sua Localizacao(Existe um ficheiro com todas as cidades):\n->");
						scanf("%[a-zA-Z -]", local);//expressão regular para todos os caracteres de a a z, Espaço e - 
						while ((c = getchar()) != '\n' && c != EOF); //clear "Enters"
						system("cls");

						ws_result = send(s, local, strlen(local), 0);
						if (ws_result < 0)//caso de erro (nao enviou localizacao)
						{
							cont = 0;

						}
					}
				}
			}
		
		}
	}

	while (cont) {
		printf("\n");
		printf(" -----------\n");
		printf("| 1) Teatros |\n| 2) Comprar |\n| 0) Sair    |\n"); //Menu
		printf(" ------------\n->");
		scanf("%c", &x);
		while ((c = getchar()) != '\n' && c != EOF);//Limpar Enter

		switch (x) {
		case '0':
			ZeroMemory(message, 1024); //limpa o bloco da memória da variavel
			strcpy(message, "400 BYE");
			send(s, message, strlen(message), 0);//envio da messagem "400 BYE" para o servidor
			ZeroMemory(strRec, 1024);
			bytesReceived = recv(s, strRec, 1024, 0);
			if (bytesReceived == SOCKET_ERROR) {//Caso de erro
				printf("\nReceive error!\n");
				cont = 0;
				break;
			}
			if (bytesReceived > 0) {
				if (strcmp(strRec, "400 BYE") == 0) {//confirmacao
					system("cls");
					printf("\nDisconnect\n");
					cont = 0;//variavel inteira para sair do while
				}
			}

			break;
		case '1':
			system("cls"); //limpa os caracteres do terminal
			ZeroMemory(message, 1024);
			strcpy(message, "TEATRO");//Envio da messagem "TEATRO" para o servidor
			ws_result = send(s, message, strlen(message), 0);

			if (ws_result < 0)//Caso de erro
			{
				return 1;
			}

			ZeroMemory(message, 1024);	
			ZeroMemory(strRec, 1024);
			printf("A espera de acesso...\n ");
			bytesReceived = recv(s, strRec, 1024, 0);
			if (bytesReceived == SOCKET_ERROR) { //caso de erro
				printf("\nReceive error!\n");
				cont = 0;
				break;
			}
			if (bytesReceived > 0) {
				if (strcmp(strRec, "100 OK") == 0) {//confirmacao
					system("cls");
					printf("Teatros Disponiveis:\n"); 
					ZeroMemory(strRec, 1024);
					bytesReceived = recv(s, strRec, 1024, 0);//receber id do primeiro
					if (bytesReceived == SOCKET_ERROR) {//caso de erro
						printf("\nReceive error!\n");
						cont = 0;
						err = 1;
						break;
					}
					else if (bytesReceived == 0) { //caso de erro 
						err = 1;


					}

					while ((strcmp(strRec, "END") != 0)) {//"END" quando acaba de enviar os teatros
						//id nometeatro numespetaculos loc numvisitas 
						// 0      1           2         3      4
						if (contador < 4) {
							printf("%s | ", strRec);//imprime a informacao recebida
							ZeroMemory(message, 1024);
							strcpy(message, "100 OK");
							send(s, message, strlen(message), 0);
							contador++; //incrementa no contador
							ZeroMemory(strRec, 1024);
							bytesReceived = recv(s, strRec, 1024, 0);

						}
						else if (contador == 4) {
							contador = 0; //reset do contador
							printf("%s\n", strRec);//imprime o numero de visitas e passa para a proximal linha
							ZeroMemory(message, 1024);
							strcpy(message, "100 OK");
							send(s, message, strlen(message), 0);
							ZeroMemory(strRec, 1024);
							bytesReceived = recv(s, strRec, 1024, 0);

						}

					}

					ZeroMemory(message, 1024);
					strcpy(message, "100 OK");//confirmacao
					send(s, message, strlen(message), 0);


					ZeroMemory(message, 1024);
					ZeroMemory(strRec, 1024);
					bytesReceived = recv(s, strRec, 1024, 0); //receber numero de teatros ja visitados
					printf("Numero de Teatros Visitados:%s\n", strRec);
					printf("Lista dos Teatros Visitados:\n");
					
					ZeroMemory(message, 1024);
					strcpy(message, "100 OK");//confirmacao
					send(s, message, strlen(message), 0);
					
					ZeroMemory(message, 1024);
					ZeroMemory(strRec, 1024);
					bytesReceived = recv(s, strRec, 1024, 0);//receber o primeiro id 
					if (bytesReceived == SOCKET_ERROR) { //caso de erro
						printf("\nReceive error!\n");
						cont = 0;
						break;
					}
					else if (bytesReceived == 0) { //caso de erro 
						err = 1;


					}

					while ((strcmp(strRec, "END") != 0)) {//"END" quando acaba de enviar os teatros
						//id nometeatro numespetaculos loc numvisitas
						// 0         1          2       3        4
						if (contador < 4) {
							printf("%s | ", strRec);//imprime a informacao recebida
							ZeroMemory(message, 1024);
							strcpy(message, "100 OK");
							send(s, message, strlen(message), 0);
							contador++; //incrementa no contador
							ZeroMemory(strRec, 1024);
							bytesReceived = recv(s, strRec, 1024, 0);

						}
						else if (contador == 4) {
							contador = 0; //reset do contador
							printf("%s\n", strRec);//imprime o numero de visitas e passa para a proximal linha
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
				else//Caso de erro
				{
					cont = 0;
					break;

				}
			}
			break;

		case '2':
			system("cls");
			ZeroMemory(message, 1024);
			strcpy(message, "COMPRA");//Envio da Opcao
			ws_result = send(s, message, strlen(message), 0);

			if (ws_result < 0)//caso de erro
			{
				puts("Send failed");
				return 1;
			}

			ZeroMemory(message, 1024);
			ZeroMemory(strRec, 1024);
			bytesReceived = recv(s, strRec, 1024, 0);
			if (bytesReceived == SOCKET_ERROR) {//Caso de erro
				printf("\nReceive error!\n");
				cont = 0;
				break;
			}
			if (bytesReceived > 0) {
				if (strcmp(strRec, "100 OK") == 0) {//confirmacao
					ZeroMemory(message, 1024);
					ZeroMemory(strRec, 1024);
					printf("A espera de acesso...\n ");//se estivermos na fila para o servidor enviar informacao
					
					system("cls");
					printf("\n");
					ZeroMemory(strRec, 1024);
					bytesReceived = recv(s, strRec, 1024, 0);//receber o primeiro id
					if (bytesReceived == SOCKET_ERROR) {//caso de erro
						printf("\nReceive error!\n");
						cont = 0;
						err = 1;
						break;
					}
					else if (bytesReceived == 0) {//caso de erro
						err = 1;


					}
					//imprime teatros (como estamos na compra imprime os teatros que nao foram visitados)
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

					if (bytesReceived == SOCKET_ERROR) {//Caso de erro
						printf("\nReceive error!\n");
						cont = 0;
						err = 1;
						break;
					}
					else if (bytesReceived == 0) {//Caso de erro
						err = 1;
					}

					if (bytesReceived > 0) {
						if (strcmp(strRec, "100 OK") == 0) {
							printf("\n-1 para Sair");
							printf("\nSelecione o numero do Teatro que ja visitou:\n->");
							scanf("%d", &comprar);//Escolha atraves do id
							while ((c = getchar()) != '\n' && c != EOF);

							char str[4];
							memset(str, '\0', 4);
							itoa(comprar, str, 10);
							send(s, str, strlen(str), 0);//envia a opcao escolhida (string)

							ZeroMemory(strRec, 1024);
							bytesReceived = recv(s, strRec, 1024, 0);

							if (bytesReceived == SOCKET_ERROR) {//Caso de erro
								printf("\nReceive error!\n");
								cont = 0;
								err = 1;
								break;
							}
							else if (bytesReceived == 0) {//Caso de erro
								err = 1;
							}

							if (bytesReceived > 0) {
								printf("\n%s\n", strRec);
								if (strcmp(strRec, "100 OK") == 0) {//Id válido
									system("cls");
									printf("Compra efetuada com sucesso!\n\n");
								}
								if (strcmp(strRec, "300 LEFT") == 0) {//Opcao -1 
									system("cls");
									printf("Pedido de compra cancelada!\n\n");
								}
								if (strcmp(strRec, "404 NOT FOUND") == 0) {//Id inválido
									system("cls");
									printf("Pedido de compra invalido. A opcao que inseriu nao existe!\n\n");
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