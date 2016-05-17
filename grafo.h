#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int tamanhoAresta;
int tamanhoVertice;
int totalAresta;
int totalVertice;


typedef struct{
	int id;	
	int quantidadeFormigas;
	formiga *listaFormiga;	
} Vertice;

struct Aresta {
	int origem;
	int destino;
	float peso;
	float feromonio;
	struct Aresta *proximaAresta;
};

typedef struct Aresta aresta;



//OPERACOES ARESTA
void iniciaAresta(aresta *lista){
	lista->proximaAresta = NULL;
	tamanhoAresta = 0;
}

int vaziaAresta(aresta *lista){
	if(lista->proximaAresta == NULL)
		return 1;
	else
		return 0;
}

aresta *alocaAresta(int vertice1, int vertice2){
	aresta *novo = (aresta *) malloc(sizeof(aresta));
	if(!novo){
		printf("Sem memoria disponivel!\n");
		exit(1);
	} else {
		novo->origem = vertice1;
		novo->destino = vertice2;
		return novo;
	}
}

void insereArestaFim(aresta *lista, int vertice1, int vertice2){
	aresta *novo = alocaAresta(vertice1, vertice2);
	novo->proximaAresta = NULL;
	if(vaziaAresta(lista)){
		lista->proximaAresta = novo;
	} else {
		aresta *tmp = lista->proximaAresta;
		while(tmp->proximaAresta != NULL){
			tmp = tmp->proximaAresta;
		}
		tmp->proximaAresta = novo;
	}
}

void mostraAresta(aresta *lista){
	system("clear");
	if(vaziaAresta(lista)){
		printf("lista vazia!\n\n");
		return ;
	}
	
	aresta *tmp;
	tmp = lista->proximaAresta;
	printf("lista: \n");
	while( tmp != NULL){
		printf("Origem  => %d \nDestino => %d \n", tmp->origem, tmp->destino);
		//printf("ID Proxima Formiga => %d\n\n", tmp->proximaFormiga->membro.id);
		tmp = tmp->proximaAresta;
	}
	
		
	printf("\n\n");
}

int existeAresta(aresta *lista, int vertice1, int vertice2){
	aresta *tmp = lista->proximaAresta;
	//printf("vertice1: %d -- vertice2: %d\n", vertice1, vertice2);
	while(tmp != NULL){
		//printf("origem: %d -- destino: %d\n", tmp->origem, tmp->destino);
		if(tmp->origem == vertice1 && tmp->destino == vertice2){
			//printf("primeiroif\n");
			return 1;
		}
		if(tmp->origem == vertice2 && tmp->destino == vertice1){
			//printf("segundoif\n");
			return 1;
		}
		tmp = tmp->proximaAresta;
	}
	return 0;
}
