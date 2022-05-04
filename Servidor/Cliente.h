#pragma once
#include <iostream>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <windows.h>
#include <thread>
#include <string>
#include <list>
#include <mutex>

class Client {
private:
	static std::mutex m;

	std::thread mThread;

	SOCKET socket = NULL;

	struct sockaddr_in* cli = NULL;

	std::string location{};

	std::list<int> teatroVisitados;
	
public:
	Client(struct sockaddr_in* cli, SOCKET socket);
	Client(struct sockaddr_in* cli, SOCKET socket, std::string location, std::list<int> teaVis);
	inline std::thread& getThread() {return mThread;}

	inline void setSocket(SOCKET v) { socket = v;}
	inline SOCKET getSocket() {return socket;}

	inline void setCli(struct sockaddr_in* v) { cli = v;}
	inline struct sockaddr_in* getCli() {return cli;}

	inline void setLocation(std::string v) {location.assign(v);}
	inline std::string getLocation() {return location;}

	inline void setTeatroVisitados(std::list<int> v) { teatroVisitados = v;}
	inline std::list<int>& getTeatrosVisitados() {return teatroVisitados;}

	static Client* CheckClient(struct sockaddr_in* _cli, SOCKET _socket);
	void UpdateClient();
	void addClientToFile();

	static std::mutex& getMutex() { return m; }

	template <typename T>
	bool contains(std::list<T>& listOfElements, const int element)
	{
		// Find the iterator if element in list
		auto it = std::find(listOfElements.begin(), listOfElements.end(), element);
		//return if iterator points to end or not. It points to end then it means element
		// does not exists in list
		return it != listOfElements.end();
	}
};
