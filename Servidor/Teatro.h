#pragma once
#include <iostream>
#include <algorithm>
#include <mutex>
#include <windows.h>
#include <vector>
#include "Cidade.h"
#include "Cliente.h"

class Teatro
{
private:
	static std::mutex m;
	static std::vector<Teatro> teatros;
	int id;
	std::string nome;
	int espetaculos;
	std::string localizacao;
	int visitas;

public:
	Teatro(int id, std::string nome, int espetaculos, std::string loc, int visitas);
	static void quicksortTeatros(const std::vector<Teatro>::iterator arr, unsigned int length, float lat, float longi);
	static void InitTeatros(float lat, float longi);
	static std::vector<Teatro>& getTeatros() { return teatros; }
	int getId() { return id; }
	std::string getNome() { return nome; }
	int getEspetaculos() { return espetaculos; }
	std::string getLocalizacao() { return localizacao; }
	int getVisitas() { return visitas; }
	static std::mutex& getMutex() { return m; }
	bool operator==(const int& i);
};


