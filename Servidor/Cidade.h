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
	static void InitCidades();
	static std::list<Cidade>& getCidades() { return cidades; }
	std::string setNome(std::string _nome) { return nome.assign(_nome); }
	float setLat(float _lat) { return lat = _lat; }
	float setLong(float _longi) { return longi = _longi; }
	std::string getNome() { return nome; }
	float getLat() { return lat; }
	float getLong() { return longi; }
	static std::mutex& getMutex() { return m; }

	template <typename T>
	static bool contains(std::list<T>& listOfElements, const std::string element)
	{
		// Find the iterator if element in list
		auto it = Cidade::find(listOfElements.begin(), listOfElements.end(), element);
		//return if iterator points to end or not. It points to end then it means element
		// does not exists in list
		return it != listOfElements.end();
	}

	template<class InputIterator>
	static InputIterator find(InputIterator first, InputIterator last, const std::string val)
	{
		std::string s1{};
		std::string s2{};
		while (first != last) {
			s1.assign((*first).getNome());
			s2.assign(val);
			std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
			std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);
			if (s1.compare(s2) == 0) return first;
			++first;
		}
		return last;
	}
};