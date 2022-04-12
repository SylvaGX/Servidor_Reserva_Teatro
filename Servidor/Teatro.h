#pragma once
#include <windows.h>
#include "Cidade.h"
#include "Cliente.h"

struct _Teatro
{
	int id;

	char nome[100];

	int espetaculos;

	char localizacao[100];

	int visitas;
};

typedef struct _Teatro Teatro;

HANDLE MutexTeatros;

Teatro** teatros;
int tamTeatros;

Teatro* Create_Teatro(int id, char nome[], int espetaculos, char loc[], int visitas);
int binarySearchTeatro(Teatro** arr, int l, int r, char loc[]);
int binarySearchTeatroIds(int* arr, int l, int r, int id);
void quicksortTeatros(const Teatro** arr, unsigned int length, float lat, float longi);
Teatro** InitTeatros(float lat, float longi, Client *c);
int isTeatroVisitado(int id, Client *c);
int getId(Teatro* t);
char* getTeatroNome(Teatro* t);
int getEspetaculos(Teatro* t);
char* getLocalizacao(Teatro* t);
int getVisitas(Teatro* t);

