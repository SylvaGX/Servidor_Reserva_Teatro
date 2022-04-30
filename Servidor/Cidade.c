#include "Cidade.h"

Cidade* Create_Cidade(char nome[], float lat, float longi) {
	Cidade *c = calloc(1, sizeof(Cidade));
	if (c != NULL)
	{
		strcpy(c->nome, nome);
		c->lat = lat;
		c->longi = longi;
	}

	return c;
}


int binarySearchCidade(Cidade** arr, int l, int r, char loc[])
{
	if (r >= l) {
		int mid = l + (r - l) / 2;

		// If the element is present at the middle
		// itself
		char cidade1[100];
		char cidade2[100];
		memset(cidade1, '\0', 100);
		memset(cidade2, '\0', 100);
		strcpy(cidade1, toUpper(getCidadeNome(arr[mid])));
		strcpy(cidade2, toUpper(loc));

		if (strcmp(cidade1, cidade2) == 0)
			return mid;

		// If element is smaller than mid, then
		// it can only be present in left subarray
		if (strcmp(cidade1, cidade2) > 0)
			return binarySearchCidade(arr, l, mid - 1, loc);

		// Else the element can only be present
		// in right subarray
		return binarySearchCidade(arr, mid + 1, r, loc);
	}

	// We reach here when element is not
	// present in array
	return -1;
}

int compareCidades(const void* a, const void* b)
{
	Cidade* data1 = *((Cidade**)a);
	Cidade* data2 = *((Cidade**)b);

	char nome1[100];
	char nome2[100];

	strcpy(nome1, toUpper(getCidadeNome(data1)));
	strcpy(nome2, toUpper(getCidadeNome(data2)));

	if (strcmp(nome1, nome2) < 0) // a < b
		return -1;
	else if (strcmp(nome1, nome2) == 0) // a == b
		return 0;
	else
		return 1;  // a > b
}

/**
* Função para ler o ficheiro das cidades e guardar num array de estruturas de cidades
* @return Cidade** - Array de estruturas de cidades
*/
Cidade** InitCidades()
{
	Cidade** cidades = NULL;
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
		tamCidades = 0;
		Cidade** auxCidades;
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

				tamCidades++;
				auxCidades = calloc(tamCidades, sizeof(Cidade*));
				if (auxCidades != NULL)
				{

					for (unsigned int i = 0; i < (tamCidades-1); i++)
					{

						auxCidades[i] = cidades[i];

					}
					free(cidades);
					auxCidades[tamCidades - 1] = Create_Cidade(nome, lat, longi);

					cidades = auxCidades;
					auxCidades = NULL;
				}
			}

			memset(nome, '\0', 100);
			tamNome = 0;
			lat = 0.0f;
			longi = 0.0f;
		}

		fclose(citys);
	}

	return cidades;
}

char* getCidadeNome(Cidade *c)
{
	if (c != NULL)
		return c->nome;
	return NULL;
}

float getLat(Cidade *c)
{
	if (c != NULL)
		return c->lat;
	return 0.0f;
}

float getLong(Cidade *c)
{
	if (c != NULL)
		return c->longi;
	return 0.0f;
}
