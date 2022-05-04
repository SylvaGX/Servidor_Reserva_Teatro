#include "Cidade.h"

std::mutex Cidade::m{};
std::list<Cidade> Cidade::cidades{};

Cidade::Cidade(std::string nome, float lat, float longi) :
	nome(nome), lat(lat), longi(longi) {}

bool Cidade::compareCidades(Cidade& a, Cidade& b)
{

	std::string nome1{};
	std::string nome2{};
	std::transform(a.getNome().begin(), a.getNome().end(), nome1.begin(), ::toupper);
	std::transform(b.getNome().begin(), b.getNome().end(), nome2.begin(), ::toupper);

	
	return (nome1.compare(nome2) < 0);
}

/**
* Função para ler o ficheiro das cidades e guardar num array de estruturas de cidades
*/
void Cidade::InitCidades()
{
	FILE* citys;

	citys = fopen("cidades.csv", "r");

	if (citys != NULL) {
		char nome[100];
		memset(nome, '\0', 100);
		int tamNome = 0;
		char c = 0;
		float lat = 0.0f;
		float longi = 0.0f;

		int notEOF = 1;
		int isTrue = 1;
		while (notEOF)
		{
			isTrue = 1;
			while (isTrue)
			{
				c = fgetc(citys);
				switch (c)
				{
				case EOF:
					isTrue = 0;
					notEOF = 0;
					break;
				case ',':
					isTrue = 0;
					break;
				default:
					tamNome++;
					strncat(nome, &c, 1);
					break;
				}
			}


			if (notEOF) {
				fscanf(citys, "%f,%f\n", &lat, &longi);
				
				Cidade c(nome, lat, longi);

				cidades.push_back(c);
			}

			memset(nome, '\0', 100);
			tamNome = 0;
			lat = 0.0f;
			longi = 0.0f;
		}

		fclose(citys);
	}
}
