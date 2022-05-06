#include <iostream>
#include <thread>
#include "server.h"

/**
* Converte graus em radianos
* @param long double degree
* @return long double -  graus em radianos
*/
long double toRadians(const long double degree)
{
	// cmath library in C++
	// defines the constant
	// M_PI as the value of
	// pi accurate to 1e-30
	long double one_deg = (M_PI) / 180;
	return (one_deg * degree);
}

/**
* Função que calcula a distancia entre latitudes e longitudes de duas cidades
* @param long double lat1
* @param long double long1
* @param long double lat2
* @param long double long2
* @return long double - Distancia entre as duas cidades
*/
long double distance(long double lat1, long double long1,
	long double lat2, long double long2)
{
	// Convert the latitudes
	// and longitudes
	// from degree to radians.
	lat1 = toRadians(lat1);
	long1 = toRadians(long1);
	lat2 = toRadians(lat2);
	long2 = toRadians(long2);

	// Haversine Formula
	long double dlong = long2 - long1;
	long double dlat = lat2 - lat1;

	long double ans = pow(sin(dlat / 2), 2) +
		cos(lat1) * cos(lat2) *
		pow(sin(dlong / 2), 2);

	ans = 2 * asin(sqrt(ans));

	// Radius of Earth in
	// Kilometers, R = 6371
	// Use R = 3956 for miles
	long double R = 6371;

	// Calculate the result
	ans = ans * R;

	return ans;
}

int main()
{
	// Inicializar winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	printf("\A Inicializar o Winsock...\n");
	int wsResult = WSAStartup(ver, &wsData);
	if (wsResult != 0) {
		fprintf(stderr, "\nWinsock setup fail! Error Code : %d\n", WSAGetLastError());
		return 1;
	}

	// Criar um socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		fprintf(stderr, "\nSocket creationg fail! Error Code : %d\n", WSAGetLastError());
		return 1;
	}

	// Ler o ficheiro das cidades e guardar num array de estruturas
	Cidade::InitCidades();

	// Ordenar o array das cidades
	Cidade::getCidades().sort([](Cidade& a, Cidade& b) {
			std::string nome1(a.getNome());
			std::string nome2(b.getNome());
			std::transform(nome1.begin(), nome1.end(), nome1.begin(), ::toupper);
			std::transform(nome2.begin(), nome2.end(), nome2.begin(), ::toupper);

			return (nome1.compare(nome2) < 0);
		});
	
	std::cout << "Socket Criado.\n";

	// Bind the socket (ip address and port)
	struct sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(DS_TEST_PORT);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (struct sockaddr*)&hint, sizeof(hint));

	// Setup the socket for listening
	listen(listening, SOMAXCONN);

	// Wait for connection
	struct sockaddr_in client;
	int clientSize = sizeof(client);

	// Main program loop
	SOCKET clientSocket;

	std::list<Client> clientes;
	Client* c;
	bool isLocOk = true;

	// Ficar a espera de um cliente
	while ((clientSocket = accept(listening, (struct sockaddr*)&client, &clientSize))) {
		// Criar uma nova thread para o cliente aceite.
		if ((c = Client::CheckClient(&client, clientSocket)) == nullptr)
		{
			char revMsg[1024];
			std::string logMsg;
			int bytesReceived = 0;
			std::thread::id myid = std::this_thread::get_id();
			std::stringstream ss{};
			ss << myid;
			std::string threadId = ss.str();

			c = new Client(&client, clientSocket);
			sendData(c, "500 LOC");
			bool isLocNotOK = true;

			while (isLocNotOK)
			{
				memset(revMsg, '\0', 1024); // Limpar a variavel

				bytesReceived = recv(c->getSocket(), revMsg, 1024, 0); // Ficar à espera de uma resposta do cliente

				// Erro inesperado do cliente
				if (bytesReceived == SOCKET_ERROR) {
					isLocOk = false;

					logMsg = "Receive error! Thread: " + threadId; // Copiar a string para a variável

					Log::WarningLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
					break;
				}

				// Cliente desconectado
				if (bytesReceived == 0) {
					isLocOk = false;

					logMsg = "Cliente Desconectado! Thread: " + threadId; // Copiar a string para a variável

					Log::InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
					break;
				}

				// A thread recebeu informação sem erro
				if (bytesReceived > 0) {

					if (Cidade::contains(Cidade::getCidades(), revMsg)) {
						c->setLocation(revMsg);

						c->addClientToFile();

						isLocNotOK = false;
						sendData(c, "100 OK");
					}
					else
						sendData(c, "404 NOT FOUND");
				}
			}
		}
		else
		{
			sendData(c, "100 OK");
		}

		if (c != NULL && isLocOk)
		{
			c->getThread() = std::thread(ClientSession, (Client*)c);
			c = NULL;
		}
		else {
			printf("Error to connect to client\n");
			delete c;
		}

		isLocOk = 1;
	}

	// Fechar o listening socket
	closesocket(listening);

	// Limpar o winsock
	WSACleanup();
}