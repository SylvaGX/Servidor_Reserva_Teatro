#include "Teatro.h"

Teatro* Create_Teatro(int id, char nome[], int espetaculos, char loc[], int visitas)
{
	Teatro* teatro = calloc(1, sizeof(Teatro));

	if (teatro != NULL)
	{

		teatro->id = id;
		strcpy(teatro->nome, nome);
		teatro->espetaculos = espetaculos;
		strcpy(teatro->localizacao, loc);
		teatro->visitas = visitas;

	}

	return teatro;
}

int binarySearchTeatro(Teatro** arr, int l, int r, char teat[])
{
	if (r >= l) {
		int mid = l + (r - l) / 2;

		char teatro1[100];
		char teatroTarget[100];

		strcpy(teatro1, toUpper(getCidadeNome(arr[mid])));
		strcpy(teatroTarget, toUpper(getCidadeNome(teat)));

		// If the element is present at the middle
		// itself
		if (strcmp(teatro1, teatroTarget) == 0)
			return mid;

		// If element is smaller than mid, then
		// it can only be present in left subarray
		if (strcmp(teatro1, teatroTarget) > 0)
			return binarySearchTeatro(arr, l, mid - 1, teat);

		// Else the element can only be present
		// in right subarray
		return binarySearchTeatro(arr, mid + 1, r, teat);
	}

	// We reach here when element is not
	// present in array
	return -1;
}

int binarySearchTeatroIds(int* arr, int l, int r, int id)
{
	if (r >= l) {
		int mid = l + (r - l) / 2;

		// If the element is present at the middle
		// itself
		if (arr[mid] == id)
			return mid;

		// If element is smaller than mid, then
		// it can only be present in left subarray
		if (arr[mid] > id)
			return binarySearchTeatroIds(arr, l, mid - 1, id);

		// Else the element can only be present
		// in right subarray
		return binarySearchTeatroIds(arr, mid + 1, r, id);
	}

	// We reach here when element is not
	// present in array
	return -1;
}

void quicksortTeatros(const Teatro** arr, unsigned int length, float lat, float longi)
{
	unsigned int i, piv = 0;
	int index1 = 0;
	int index2 = 0;

	if (length <= 1)
		return;

	for (i = 0; i < length; i++) {
		// if curr str < pivot str, move curr into lower array and  lower++(pvt)
		index1 = binarySearchCidade(cidades, 0, tamCidades, getLocalizacao(arr[i]));
		index2 = binarySearchCidade(cidades, 0, tamCidades, getLocalizacao(arr[length - 1]));
		if (distance(getLat(cidades[index1]), getLong(cidades[index1]), lat, longi) < distance(getLat(cidades[index2]), getLong(cidades[index2]), lat, longi)) 	//use string in last index as pivot
			swap(arr + i, arr + piv++);
	}
	//move pivot to "middle"
	swap(arr + piv, arr + length - 1);

	//recursively sort upper and lower
	quicksortTeatros(arr, piv++, lat, longi);			//set length to current pvt and increase for next call
	quicksortTeatros(arr + piv, length - piv, lat, longi);
}

/**
* Função para ler o ficheiro dos teatros e guardar num array de estruturas de teatros
* @param long double lat
* @param long double longi
* @param Client* client
* @return Teatro** - Array de estruturas de teatros
*/
Teatro** InitTeatros(float lat, float longi, Client *client)
{

	Teatro** teatros = NULL;
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
		tamTeatros = 0;
		Teatro** auxTeatros;
		while (notEOF)
		{

			if (feof(theaters)) {
				//printf("Ola\n");
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

					tamTeatros++;
					auxTeatros = calloc(tamTeatros, sizeof(Teatro*));
					if (auxTeatros != NULL)
					{
						for (unsigned int i = 0; i < (tamTeatros-1); i++)
						{

							*(auxTeatros + i) = *(teatros + i);

						}

						free(teatros);

						*(auxTeatros + (tamTeatros - 1)) = Create_Teatro(id, nome, espetaculos, loc, visitas);
						teatros = auxTeatros;
						auxTeatros = NULL;
					}
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

	quicksortTeatros(teatros, tamTeatros, lat, longi);

	return teatros;

}

int isTeatroVisitado(int id, Client *c)
{

	if (c != NULL)
	{
		int aux = -1;

		if (c->tamTeatrosVisitados > 0)
		{

			aux = binarySearchTeatroIds(c->teatroVisitados, 0, c->tamTeatrosVisitados, id);
			return (aux == -1) ? id : -1;

		}

		return id;
	}
	
	return -1;
}

int getId(Teatro* t)
{
	if (t != NULL)
		return t->id;

	return -1;
}

char* getTeatroNome(Teatro* t)
{
	if (t != NULL)
		return t->nome;

	return NULL;
}

int getEspetaculos(Teatro* t)
{
	if (t != NULL)
		return t->espetaculos;

	return -1;
}

char* getLocalizacao(Teatro* t)
{
	if (t != NULL)
		return t->localizacao;

	return NULL;
}

int getVisitas(Teatro* t)
{
	if (t != NULL)
		return t->visitas;

	return -1;
}
