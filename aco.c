#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "formiga.h"
#include "grafo.h"
#include "leituraArquivo.h"

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

char nome_arquivo[60];

int main(int argc, char *argv[]){
	//printf("%s\n", argv[0]);
	strcpy(nome_arquivo, argv[1]);
	leituraArquivo(nome_arquivo);

	//printf("%s\n", nome_arquivo);
	//printf("%s\n", argv[2]);
}

/*
int main(int argc, char **argv){
	int opt;
	printf("Iniciou o programa!\n");
	if(argc < 2){
		printf("Entre	 com as entradas... \n");
		return 0;
	}
	int oi;
	while( (opt = getopt(argc, argv, "hn:i:e:c:a")) > 0 ) {
		switch(opt){
			case 'a':
			oi = (int) getopt;
			break;
		}
	}

	printf("%d\n", oi);

	//vertice *vert = (vertice *) malloc(sizeof(vertice));
	//vert->listaFormiga = (formiga *) malloc(sizeof(formiga));
	formiga *lista = (formiga *) malloc(sizeof(formiga));
	/ *if(!lista){
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

	free(vert->listaFormiga); tem '* /' aqui
	
	aresta *arestasOriginais = (aresta * ) malloc(sizeof(aresta));

	leituraEntrada(arestasOriginais);
	mostraAresta(arestasOriginais);
	//printf("Procura 1 => %d\n", existeAresta(arestasOriginais, 0, 1));
	//printf("Procura 2 => %d\n", existeAresta(arestasOriginais, 1, 0));
	//printf("Procura 3 => %d\n", existeAresta(arestasOriginais, 0, 0));
	//printf("Procura 4 => %d\n", existeAresta(arestasOriginais, 4, 1));
	//printf("Procura 5 => %d\n", existeAresta(arestasOriginais, 1, 4));
	//printf("Procura 6 => %d\n", existeAresta(arestasOriginais, 5, 4));

	coloniaSystem(lista);
	
	free(lista);
	printf("Programa Encerrado!\n");
	return 0;
}
*/