#include <stdio.h>
#include <stdlib.h>

int tamanhoFormiga;

/*typedef struct{
int id;
} dadosFormiga;*/

typedef struct{
	int qtdVertice;
	int qtdVerticeIndisponiveis;
	int verticeRestantes;
	int * listaVertice;
} Formiga;
