#pragma once
#include <iostream>
#include <algorithm>
#include <string>
#include <mutex>
#include <windows.h>
#include <list>
#include "Main.h"

class Cidade
{
private:
	static std::mutex m;
	static std::list<Cidade> cidades;
	std::string nome;
	float lat;
	float longi;
public:
	Cidade(std::string nome, float lat, float longi);
	bool compareCidades(Cidade& a, Cidade& b);
	static void InitCidades();
	static std::list<Cidade>& getCidades() { return cidades; }
	std::string setNome(std::string _nome) { return nome.assign(_nome); }
	float setLat(float _lat) { return lat = _lat; }
	float setLong(float _longi) { return longi = _longi; }
	std::string getNome() { return nome; }
	float getLat() { return lat; }
	float getLong() { return longi; }
	static std::mutex& getMutex() { return m; }

	template<class InputIterator>
	static InputIterator find(InputIterator first, InputIterator last, const std::string val)
	{
		while (first != last) {
			if ((*first).getNome().compare(val) == 0) return first;
			++first;
		}
		return last;
	}
};