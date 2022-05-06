#pragma once
#include <sstream>
#include <winsock2.h>
#include <time.h>
#include <process.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Log.h"
#include "Cidade.h"
#include "Cliente.h"
#include "Teatro.h"
#define TRUE 1
//#define DS_TEST_PORT 80
#define DS_TEST_PORT 45370

#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable : 4996)

void __stdcall ClientSession(Client*);
int sendData(Client* client, std::string data);