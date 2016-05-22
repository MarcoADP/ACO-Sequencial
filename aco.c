#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "formiga.h"
#include "grafo.h"
//#include "leituraArquivo.h"

//#define edge(x,y) (bitmap[y/CHARBITS][x] & (1<<(y%CHARBITS)))
#define INT 32		/* computer word size */
#define CHARBITS 8
#define edge(x,y) (bitmap[y/CHARBITS][x] & (1<<(y%CHARBITS)))

#define NMAX 3600		/* maximum number of vertices handles */
#define MAX_NR_VERTICES		3600	/* = NMAX */
#define MAX_NR_VERTICESdiv8	450	/* = NMAX/8 */
#define BOOL	char
#define MAX_PREAMBLE 10000
#define REORDER

/*
A LEITURA DAS INSTANCIAS FOI BASEADA NAS SOLUCOES DISPONIVEIS EM:
	ftp://dimacs.rutgers.edu/pub/challenge/graph/solvers/
*/

double *vetorVertice;
int *vetorResposta;
int NumeroFormigas = 100;
Formiga *listaFormiga;

double alpha = 0.5;
double beta = 0.5;

unsigned mask[INT] =
   {
   1, 1<<31, 1<<30, 1<<29, 1<<28, 1<<27, 1<<26, 1<<25, 1<<24,
   1<<23, 1<<22, 1<<21, 1<<20, 1<<19, 1<<18, 1<<17, 1<<16,
   1<<15, 1<<14, 1<<13, 1<<12, 1<<11, 1<<10, 1<<9, 1<<8,
   1<<7, 1<<6, 1<<5, 1<<4, 1<<3, 1<<2, 1<<1
   };  /* CAUTION - assumes 32 bit machine */

  /* graph input parameters */

int Nr_vert, Nr_edges;
BOOL Bitmap[MAX_NR_VERTICES][MAX_NR_VERTICESdiv8];
static char Preamble[MAX_PREAMBLE];
char masks[ 8 ] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

unsigned char bitmap[NMAX/CHARBITS+1][NMAX+1];
int N;			/* number of vertices in graph */

BOOL get_edge(int i, int j ){
	int byte, bit;
	char mask;
	int k;

	if (i<j) {
		k = i;
		i = j;
		j = k;
	}
	
	bit  = 7-(j & 0x00000007);
	byte = j >> 3;
	
	mask = masks[bit];
	return( (Bitmap[i][byte] & mask)==mask );
}

int get_params(){

	char c, *tmp;
	char * pp = Preamble;
	int stop = 0;
	tmp = (char *)calloc(100, sizeof(char));
	
	Nr_vert = Nr_edges = 0;
	
	while (!stop && (c = *pp++) != '\0'){
		switch (c)
		  {
			case 'c':
			  while ((c = *pp++) != '\n' && c != '\0');
			  break;
			  
			case 'p':
			  sscanf(pp, "%s %d %d\n", tmp, &Nr_vert, &Nr_edges);
			  stop = 1;
			  break;
			  
			default:
			  break;
		  }
	}
	
	free(tmp);
	
	if (Nr_vert == 0 || Nr_edges == 0)
	  return 0;  /* error */
	else
	  return 1;
}

void readgraph(FILE* fp){
   int i,j;
   int length = 0;

	if (!fscanf(fp, "%d\n", &length))
	  { printf("ERROR: Corrupted preamble.\n"); exit(10); }

	if(length >= MAX_PREAMBLE)
	  { printf("ERROR: Too long preamble.\n"); exit(10); }
		   
	fread(Preamble, 1, length, fp);
	Preamble[length] = '\0';
	
	if (!get_params())
		  { printf("ERROR: Corrupted preamble.\n"); exit(10); }

	if (Nr_vert >NMAX) {
		printf("Too many vertices! Recompile with NMAX > %d\n",
			Nr_vert);
		exit(0);
	}

	for ( i = 0
		 ; i < Nr_vert && fread(Bitmap[i], 1, (int)((i + 8)/8), fp)
		 ; i++ );

	fclose(fp);

   N = Nr_vert;

   for (i = 0; i < N; i++)
       for (j = 0; j < N; j++)
	   if (get_edge(i, j)){
	       bitmap[(j+1)/CHARBITS][i+1] |= (1 << ((j+1) % CHARBITS));
		}
		
	/*int ii, jj, total = 0;
	for(ii = 1; ii <= Nr_vert; ii++){
		for(jj = 1; jj <= Nr_vert; jj++){
			if(edge(ii, jj) && ii > jj){
				printf("%d -- %d\n", ii, jj);
				total++;
			}
		}
	}*/
	//	printf("%d -- %d\n", total, Nr_edges);
}

void inicializarAlgoritmo(){
	int i;
	vetorResposta = (int *) calloc(Nr_vert, sizeof(int));
	vetorVertice = (double *) calloc(Nr_vert, sizeof(double));
	listaFormiga = (Formiga *) calloc(NumeroFormigas, sizeof(Formiga));

	for(i = 0; i < NumeroFormigas; i++){
		listaFormiga[i].qtdVertice = 0;
		listaFormiga[i].qtdVerticeIndisponiveis = 0;
		listaFormiga[i].verticeRestantes = Nr_vert;
		listaFormiga[i].listaVertice = (int *) calloc(Nr_vert, sizeof(int));
	}
}

void mostraFormiga(Formiga *formigaAtual){
	int i;
	printf("\n Sobre Vertices \n");
	for(i = 0; i < Nr_vert; i++){
		int vertice = i + 1;
		if(formigaAtual->listaVertice[i] == 0){
			printf("%d -- %d\n", vertice, formigaAtual->listaVertice[i]);
		}
	}
	printf("Vertices Totais   => %d\n", Nr_vert);
	printf("Vertices Na Resposta   => %d\n", formigaAtual->qtdVertice);
	printf("Vertices Indisponiveis => %d\n", formigaAtual->qtdVerticeIndisponiveis);
	printf("Vertices Restantes => %d\n\n", formigaAtual->verticeRestantes);
}

void atualizaFormiga(Formiga *formigaAtual, int indice){
	vetorResposta[formigaAtual->qtdVertice] = indice+1;
	formigaAtual->listaVertice[indice] = 1;
	formigaAtual->qtdVertice++;
	formigaAtual->verticeRestantes--;
}

void invalidaAdjacentes(Formiga *formigaAtual, int vertice){
	int i;
	//printf("Arestas:\n");
	for(i = 1; i <= Nr_vert; i++){
		int indice = i - 1;
		if(edge(vertice, i) && formigaAtual->listaVertice[indice] == 0){
			//printf("%d -- %d\n", vertice, i);
			int indice = i - 1;
			formigaAtual->listaVertice[indice] = -1;
			formigaAtual->qtdVerticeIndisponiveis++;
			formigaAtual->verticeRestantes--;
		}
	}
}

double calculaFeromonio(int vertice){
	int indice = vertice - 1;
	return vetorVertice[indice];
}

double calculaHeuristica(Formiga *formigaAtual, int vertice){
	int i;
	int tamanho = 0;
	//printf("\n");
	for(i = 1; i<= Nr_vert; i++){
		int indice_i = i - 1;
		if(formigaAtual->listaVertice[indice_i] == 0){ 
			
			//se for != é porque o Vertice ja foi usado
			//1 se esta na resposta
			//-1 se contem aresta com um vertice que esta na resposta

			if(!edge(vertice, i) && vertice != i){
				//printf("%d -- ", i);
				tamanho++;
			}

		}
	}
	return (double) tamanho;
}

double calculaFuncao(Formiga *formigaAtual, int vertice){
	double resultado = 0.0;
	double resultadoFeromonio = calculaFeromonio(vertice);
	double resultadoHeuristica = calculaHeuristica(formigaAtual, vertice);

	resultadoFeromonio = pow(resultadoFeromonio, alpha);
	resultadoHeuristica = pow(resultadoHeuristica, beta);

	resultado = resultadoFeromonio + resultadoHeuristica;
	return resultado;
}

int escolheVertice(Formiga *formigaAtual){
	int i;
	//int a = 0;
	int indice;
	int VerticeEscolhido = -1;
	double valorMAX = 0.0;
	//printf("\n DISPONIVEIS => ");
	for(i = 1; i <= Nr_vert; i++){
		indice = i - 1;
		if(formigaAtual->listaVertice[indice] == 0){
			//printf(" %d ", i);
			double valor = calculaFuncao(formigaAtual, i);
			//printf("Vertice => %d -- Funcao => %lf\n", i, valor);
			if(valor >= valorMAX){
				//printf("Vertice => %d -- Funcao => %lf\n", i, valor);
				valorMAX = valor;
				VerticeEscolhido = i;
			}
		} /*else {
			printf("Indisponiveis => %d\n", i);
			a++;
		}*/
	}
	//printf("Vertice Escolhido => %d -- %lf\n", VerticeEscolhido, valorMAX);
	//printf("Total => %d\n", a);
	return VerticeEscolhido;
}

void construirSolucao(Formiga *formigaAtual){
	/*int i, j, a = 0;
	for(i = 1; i <= Nr_vert; i++){
		for(j = i + 1; j <= Nr_vert; j++){
			if(edge(i,j)){
				printf("%d -- %d\n", i, j);
				a++;
			}
		}
	}
	printf("%d -- %d\n", a, Nr_edges);*/
	int numeroRandom = rand() %Nr_vert; // 0 a Nr_Vertices-1
	int numeroReal = numeroRandom + 1; //Vertice Real Ex: Random = 20, Vertice = 21
	//printf("NumeroRandom => %d --  NumeroReal => %d\n", numeroRandom, numeroReal);
	atualizaFormiga(formigaAtual, numeroRandom); //Coloca 1 no indice do vertice, no caso [20]
	invalidaAdjacentes(formigaAtual, numeroReal);
	//mostraFormiga(formigaAtual);
	while(formigaAtual->verticeRestantes != 0){
		int vertice = escolheVertice(formigaAtual);
		/*if(vertice == -1){
			mostraFormiga(formigaAtual);
			return;
		}*/
		int indice = vertice - 1;
		atualizaFormiga(formigaAtual, indice);
		invalidaAdjacentes(formigaAtual, vertice);
	}
	//mostraFormiga(formigaAtual);
}

void verificaResposta(Formiga *formigaAtual){
	int i, j;
	int fim = formigaAtual->qtdVertice;
	for(i = 0; i < fim; i++){
		for(j = i + 1; j < fim; j++){
			if(edge(vetorResposta[i], vetorResposta[j])){
				printf("%d -- %d \n", vetorResposta[i], vetorResposta[j]);
				printf("Resposta Incorreta!\n");
				return;
			}
		}
	}
	int soma = 0;
	for(i = 1; i <= Nr_vert; i++){
		//printf("I => %d\n", i);
		//printf("\n\n");
		soma = 0;	
		for(j = 0; j <= fim; j++){
			int vert = vetorResposta[j];
			//printf("%d -- %d\n", i, vert);
			if(i == vetorResposta[j]){
				break;
			}
			if(edge(i, vert)){
				//printf("TEM ARESTA!\n");
				break;
			} else {
				soma++;
			}
			if(soma == fim){
				printf("FUDEU %d\n", i);
			}

		}
		/*soma = 0;
		for(j = 0; j < fim; j++){
			if(!edge(i, vetorResposta[j])){
				soma++;
			}
		}
		if(soma == fim){
			printf("%d -- %d\n", i, j);
			printf("Resposta Errada!\n");
		}*/
	}
}
void mostraResposta(Formiga *formigaAtual){
	int i;
	printf("Nº Vertices => %d\n", formigaAtual->qtdVertice);
	printf("Vertices => ");
	for(i = 0; i < formigaAtual->qtdVertice; i++){
		//if(formigaAtual->listaVertice[i] == 1){
			printf(" %d ", vetorResposta[i]);
		//}
	}
	printf("\n\n");
}

void AntSystemColony(){
	int i;
	inicializarAlgoritmo();
	int melhorFormiga;
	int melhorConjunto = 0;
	srand (time (NULL));
	for(i = 0; i < NumeroFormigas; i++){
		construirSolucao(&listaFormiga[i]);
		//mostraFormiga(&listaFormiga[i]);
		//mostraResposta(&listaFormiga[i]);
		verificaResposta(&listaFormiga[i]);
		if(listaFormiga[i].qtdVertice > melhorConjunto){
			printf("%d\n", i);
			//mostraResposta(&listaFormiga[i]);
			melhorConjunto = listaFormiga[i].qtdVertice;
			melhorFormiga = i;
		}
	}
	mostraResposta(&listaFormiga[melhorFormiga]);
	/*printf("\nTotal => %d\n", listaFormiga[0].qtdVertice);
	printf("Resposta => ");
	for(i = 0; i < listaFormiga[0].qtdVertice; i++){	
		
		printf(" %d ", vetorResposta[i]+1);
	}
	printf("\n");*/

}

void mostraArestas(){
	int i, j, tam = 0;
	for(i = 0; i <= Nr_vert; i++){
		for (j = i+1; j <= Nr_vert; j++){
			if(edge(i, j)){
				printf("%d -- %d\n", i, j);
				tam++;
			}
		}
	}
	printf("%d -- %d\n", tam, Nr_edges);
}

int main(int argc, char *argv[]){
	char nome_arquivo[60];
	FILE* fp;
	//printf("%s\n", argv[0]);
	strcpy(nome_arquivo, argv[1]);
	if((fp = fopen(nome_arquivo, "r")) == NULL){
		printf("Arquivo inexistente\n");
		return 0;
	}
	readgraph(fp);
	AntSystemColony();
	//mostraFormiga(&listaFormiga[0]);
	//mostraArestas();
	//printf("%s\n", nome_arquivo);
	//printf("%s\n", argv[2]);
	return 0;
}
