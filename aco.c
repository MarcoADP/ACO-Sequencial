#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//TAXONOMIAS

//it			:	iteração/ciclo

//fer 			:	quantidade de feromonio
//custo			:	custo das arestas
//visibilidade	:	1/custo
//x, y			:	numeros pré definidos
//soma(fer)		:	soma de todos os fer dos vertices nao visitados		
//prob			:	probabilidade de escolha de uma aresta (fer/custo)
//				=	(fer^x * visibilidade^y/[somatorio(fer(nao visitados)^x * custo(nao visitados)^y)]

//taxa 			:	taxa de evaporação, taxa < 1
//quantia 		:	quantidade de feromonio depositada ao passar pela aresta
//				=	Q/L => Q constante e L comprimento do tour da formiga


//ESTRUTURAS
int tamanhoFormiga;
int tamanhoAresta;
int tamanhoVertice;
int totalAresta;
int totalVertice;

typedef struct{
	int id;
} dadosFormiga;

struct Formiga{
	dadosFormiga membro;
	struct Formiga *proximaFormiga;
};
typedef struct Formiga formiga;

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

//OPERACOES DE LISTA FORMIGA
void inicialista(formiga *lista){
	lista->proximaFormiga = NULL;
	tamanhoFormiga = 0;
}

int vazia(formiga *colonia){
	if(colonia->proximaFormiga == NULL)
		return 1;
	else
		return 0;
}

formiga *aloca(dadosFormiga novaFormiga){
	formiga *novo =(formiga *) malloc(sizeof(formiga));
	if(!novo){
		printf("Sem memoria disponivel!\n");
		exit(1);
	}else{
		//printf("Novo elemento: "); scanf("%d", &novo->num);
		novo->membro = novaFormiga;
		return novo;
	}
}

void insereFormigaFim(formiga *lista, dadosFormiga novaFormiga){
	formiga *novo = aloca(novaFormiga);
	novo->proximaFormiga = NULL;
	if(vazia(lista)){
		lista->proximaFormiga = novo;
	} else {
		formiga *tmp = lista->proximaFormiga;
		while(tmp->proximaFormiga != NULL){
			tmp = tmp->proximaFormiga;
		}

		tmp->proximaFormiga = novo;
	}
}

void mostralista(formiga *lista){
	//system("clear");
	if(vazia(lista)){
		printf("lista vazia!\n\n");
		return ;
	}
	
	formiga *tmp;
	tmp = lista->proximaFormiga;
	printf("lista: \n");
	while( tmp != NULL){
		printf("ID Formiga => %d\n", tmp->membro.id);
		//printf("ID Proxima Formiga => %d\n\n", tmp->proximaFormiga->membro.id);
		tmp = tmp->proximaFormiga;
	}
	
		
	printf("\n\n");
}

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


//ANT COLONY OPTIMIZATION
void buscaLocal(){

}

void atualizarFeromonio(){

}

void gerarSolucoes(){

}

void inicializaAlgoritmo(){

}

void coloniaSystem(formiga *listaFormigas){
	int it = 0;
	int criterioPorIteracao = 100;
	int numFormigas = 2;
	int i;
	inicializaAlgoritmo();
	while(it < criterioPorIteracao){
		for(i = 1; i <= numFormigas; i++){
			gerarSolucoes();
		}	
		it++;
	}

	atualizarFeromonio();

	buscaLocal();
}

void leituraEntrada(aresta *listaAresta){
	FILE* arq;

	arq = fopen("entrada1.txt", "r"); 
	if(arq == NULL){
		printf("Arquivo nao encontrado!\n");
		exit(1);
	} else {
		fscanf(arq, "%d %d", &totalVertice, &totalAresta);
		//printf("Vertice => %d \nAresta  => %d \n\n", totalVertice, totalAresta);
		int i, origem, destino;
		for(i = 0; i < totalAresta; i++){
			fscanf(arq, "%d %d", &origem, &destino);
			//printf("Origem  => %d \nDestino => %d \n\n", origem, destino);
			insereArestaFim(listaAresta, origem, destino);
		}
	}
	//mostraAresta(listaAresta);

}

int main(){
	
	printf("Iniciou o programa!\n");
	/*vertice *vert = (vertice *) malloc(sizeof(vertice));
	vert->listaFormiga = (formiga *) malloc(sizeof(formiga));*/
	formiga *lista = (formiga *) malloc(sizeof(formiga));
	/*if(!lista){
		printf("Sem memoria disponivel!\n");
		exit(1);
	}else{
		inicialista(vert->listaFormiga);
	}
	
	int i;
	for(i = 0; i < 10; i++){
		dadosFormiga novaFormiga;
		novaFormiga.id = 2 * i;
		insereFormigaFim(vert->listaFormiga, novaFormiga);
	}

	mostralista(vert->listaFormiga);

	free(vert->listaFormiga);*/
	
	aresta *arestasOriginais = (aresta * ) malloc(sizeof(aresta));

	leituraEntrada(arestasOriginais);
	mostraAresta(arestasOriginais);
	/*printf("Procura 1 => %d\n", existeAresta(arestasOriginais, 0, 1));
	printf("Procura 2 => %d\n", existeAresta(arestasOriginais, 1, 0));
	printf("Procura 3 => %d\n", existeAresta(arestasOriginais, 0, 0));
	printf("Procura 4 => %d\n", existeAresta(arestasOriginais, 4, 1));
	printf("Procura 5 => %d\n", existeAresta(arestasOriginais, 1, 4));
	printf("Procura 6 => %d\n", existeAresta(arestasOriginais, 5, 4));*/

	coloniaSystem(lista);
	
	free(lista);
	printf("Programa Encerrado!\n");
	return 0;
}
