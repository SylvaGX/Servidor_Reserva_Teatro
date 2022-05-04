#include "Cliente.h"
#include "Log.h"

std::mutex Client::m{};

Client::Client(struct sockaddr_in* cli, SOCKET socket) :
	cli(cli), socket(socket) {}

Client::Client(sockaddr_in* cli, SOCKET socket, std::string location, std::list<int> teaVis) :
	cli(cli), socket(socket), location(location), teatroVisitados(teaVis) {}


void Client::UpdateClient() {

	std::unique_lock<std::mutex> lck(getMutex());

	std::ifstream fin;
	std::ofstream of;
	std::string logMsg{};
	std::thread::id myid = std::this_thread::get_id();
	std::stringstream ss{};
	ss << myid;
	std::string threadId = ss.str();

	fin.open("clientes.csv");
	of.open("clientesTemp.csv");

	if (of && fin)
	{
		logMsg = "A atualizar o cliente... Thread: " + threadId; // Copiar a string para a variável

		Log::InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
		std::string strTemp;
		std::string IP;
		size_t tam = 0;
		size_t i = 0;
		while (fin >> strTemp)
		{
			IP.assign(inet_ntoa(getCli()->sin_addr));
			if (strTemp.substr(0, strTemp.find(',')).compare(IP) == 0) {
				tam = getTeatrosVisitados().size();
				i = 0;
				strTemp.clear();
				strTemp = IP + "," + getLocation() + "," + std::to_string(tam);
				strTemp += (tam > 0) ? "," : "";
				for (auto& tv : getTeatrosVisitados())
				{
					if (i == tam - 1)
					{
						strTemp += std::to_string(tv);
					}
					else
					{
						strTemp += std::to_string(tv) + ",";
					}

					i++;
				}
			}
			strTemp += "\n";
			of << strTemp;
		}

		fin.close();
		of.close();

		std::filesystem::path p = std::filesystem::current_path();
		try
		{
			std::filesystem::rename(p / "clientes.csv", p / "clientesBackup.csv");
			std::filesystem::rename(p / "clientesTemp.csv", p / "clientes.csv");
			std::filesystem::remove(p / "clientesBackup.csv");
		}
		catch (const std::exception& e)
		{
			std::string err{ e.what() };

			logMsg = "Erro ao atualizar o cliente ERR: " + err + " . Thread: " + threadId; // Copiar a string para a variável

			Log::WarningLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
		}
	}
	else
	{
		fin.close();
		of.close();

		logMsg = "Erro ao abrir os ficheiros de clientes. Thread: " + threadId; // Copiar a string para a variável

		Log::WarningLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
	}
}

void Client::addClientToFile()
{

	std::ofstream f;
	std::string logMsg{};
	std::thread::id myid = std::this_thread::get_id();
	std::stringstream ss{};
	ss << myid;
	std::string threadId = ss.str();

	f.open("clientes.csv", std::ios_base::app);

	if (f)
	{

		std::string strTemp;
		std::string IP;
		IP.assign(inet_ntoa(getCli()->sin_addr));
		strTemp = IP + "," + getLocation() + ",0";
		f << strTemp << "\n";
		
		logMsg = "Cliente atualizado com sucesso. Thread: " + threadId; // Copiar a string para a variável

		Log::InfoLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal

	}
	else
	{
		std::string err{ strerror(errno) };

		logMsg = "Erro ao guardar o cliente ERR:" + err + " . Thread: " + threadId; // Copiar a string para a variável

		Log::WarningLog(logMsg); // Fazer log da variável para um ficheiro e para o terminal
	}

	f.close();
}

Client* Client::CheckClient(struct sockaddr_in* _cli, SOCKET _socket){

	FILE* readFile;

	std::string line{};
	std::string location{};
	std::string IP{};
	std::list<int> teatroVisitados;
	char IPCharN[16];
	char loc[100];
	memset(IPCharN, '\0', 16);
	memset(loc, '\0', 100);
	int numVisitas{ 0 };
	int i{ 0 };
	int auxIdVisitas = 0;

	readFile = fopen("clientes.csv", "r");

	if (readFile != NULL)
	{
		while (!feof(readFile))
		{

			fscanf(readFile, "%[0-9.],%[a-zA-Z],%d", IPCharN, loc, &numVisitas);

			i = 0;
			if (numVisitas > 0)
			{
				fscanf(readFile, ",");
				while (true)
				{
					if (i == numVisitas - 1)
					{
						fscanf(readFile, "%d\n", &auxIdVisitas);
						teatroVisitados.push_back(auxIdVisitas);
						break;
					}
					else
					{
						fscanf(readFile, "%d,", &auxIdVisitas);
						teatroVisitados.push_back(auxIdVisitas);
					}
					i++;
				}
			}
			else
			{
				fscanf(readFile, "\n");
			}

			IP.assign(IPCharN);

			if (IP.compare(inet_ntoa(_cli->sin_addr)) == 0)
			{
				location.assign(loc);
				Client *c = new Client(_cli, _socket, location, teatroVisitados);

				fclose(readFile);

				return c;
			}
			else
			{
				location.clear();
				IP.clear();
				teatroVisitados.clear();
				numVisitas = 0;
				auxIdVisitas = 0;
			}

			memset(IPCharN, '\0', 16);
			memset(loc, '\0', 100);

			fscanf(readFile, "\n");

		}

		fclose(readFile);
	}

	return nullptr;
}