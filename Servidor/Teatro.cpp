#include "Teatro.h"

std::mutex Teatro::m{};
std::vector<Teatro> Teatro::teatros{};

Teatro::Teatro(int id, std::string nome, int espetaculos, std::string loc, int visitas) :
id(id), nome(nome), espetaculos(espetaculos), localizacao(loc), visitas(visitas) {}

void Teatro::quicksortTeatros(const std::vector<Teatro>::iterator arr, unsigned int length, float lat, float longi)
{
	unsigned int i, piv = 0;

	if (length <= 1)
		return;

	for (i = 0; i < length; i++) {
		// if curr str < pivot str, move curr into lower array and  lower++(pvt)
		auto itA = Cidade::find(Cidade::getCidades().begin(), Cidade::getCidades().end(), arr[i].getLocalizacao());
		auto itB = Cidade::find(Cidade::getCidades().begin(), Cidade::getCidades().end(), arr[length - 1].getLocalizacao());
		if (distance((*itA).getLat(), (*itA).getLong(), lat, longi) < distance((*itB).getLat(), (*itB).getLong(), lat, longi)) 	//use string in last index as pivot
			std::swap(*(arr + i), *(arr + piv++));
	}
	//move pivot to "middle"
	std::swap(*(arr + piv), *(arr + length - 1));

	//recursively sort upper and lower
	quicksortTeatros(arr, piv++, lat, longi); //set length to current pvt and increase for next call
	quicksortTeatros(arr + piv, length - piv, lat, longi);
}

/**
* Função para ler o ficheiro dos teatros e guardar num array de estruturas de teatros
* @param long double lat
* @param long double longi
*/
void Teatro::InitTeatros(float lat, float longi)
{
	FILE* theaters;

	theaters = fopen("teatros.csv", "r");

	if (theaters != NULL) {
		char nome[100];
		memset(nome, '\0', 100);
		unsigned int tamNome = 0;
		char loc[100];
		memset(loc, '\0', 100);
		int tamLoc = 0;
		char c = 0;
		int id = -1;
		int espetaculos = -1;
		int visitas = -1;

		int notEOF = 1;
		int isTrue = 1;
		while (notEOF)
		{

			if (feof(theaters)) {
				notEOF = 0;
				isTrue = 0;
				break;
			}
			
			fscanf(theaters, "%d,", &id);

			isTrue = 1;
			while (isTrue)
			{
				c = fgetc(theaters);
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
				fscanf(theaters, "%d,", &espetaculos);

				isTrue = 1;
				while (isTrue)
				{
					c = fgetc(theaters);
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
						tamLoc++;
						strncat(loc, &c, 1);
						break;
					}
				}

				if (notEOF) {

					fscanf(theaters, "%d\n", &visitas);

					Teatro t(id, nome, espetaculos, loc, visitas);

					getTeatros().push_back(t);
				}
			}

			memset(nome, '\0', 100);
			tamNome = 0;
			memset(loc, '\0', 100);
			tamLoc = 0;
			id = -1;
			espetaculos = -1;
			visitas = -1;
		}

		fclose(theaters);
	}

	quicksortTeatros(getTeatros().begin(), getTeatros().size(), lat, longi);
}

bool Teatro::operator==(const int& i)
{
	return getId() == i;
}
