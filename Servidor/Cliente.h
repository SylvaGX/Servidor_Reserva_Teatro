#pragma once
#include <windows.h>

struct _Client {

	HANDLE hThread;

	unsigned int threadID;

	SOCKET socket;

	char location[100];

	int* teatroVisitados;

	int tamTeatrosVisitados;

};

typedef struct _Client Client;