#pragma once
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "Main.h"

struct _Cidade
{
	char nome[100];
	float lat;
	float longi;
};

typedef struct _Cidade Cidade;

HANDLE MutexCidades;

Cidade** cidades;
int tamCidades;

Cidade* Create_Cidade(char nome[], float lat, float longi);
int binarySearchCidade(Cidade** arr, int l, int r, char loc[]);
int compareCidades(const void* a, const void* b);
Cidade** InitCidades();
char* getCidadeNome(Cidade* c);
float getLat(Cidade* c);
float getLong(Cidade* c);