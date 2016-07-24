#include <stdio.h>
#include <stdlib.h>
//#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "mpi.h"
#include "formiga.h"

#define INT 32    /* computer word size */
#define CHARBITS 8
#define edge(x,y) (bitmap[y/CHARBITS][x] & (1<<(y%CHARBITS)))

#define NMAX 3600    /* maximum number of vertices handles */
#define MAX_NR_VERTICES    3600  /* = NMAX */
#define MAX_NR_VERTICESdiv8   450   /* = NMAX/8 */
#define BOOL   char
#define MAX_PREAMBLE 10000
#define REORDER

//Ver como paralelizar
//Quando tem um vetor global, quando da malloc em cada thread, ele vira local para a thread?
//Vetor Feromonio é global, logo precisa inicializar uma unica vez?
//Atualizar Feromonio é área compartilhada, logo precisa de lock?
//Quando printa o Id zoa, tipo dois 2 e nenhum 1

//pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
//pthread_barrier_t barreira = PTHREAD_COND_INITIALIZER;
//pthread_barrier_t barreira;

int wrank, wsize, c, melhorRespostaVert;
MPI_Status status;

typedef struct{
    int id;
} thread_arg;

/*
A LEITURA DAS INSTANCIAS FOI BASEADA NAS SOLUCOES DISPONIVEIS EM:
ftp://dimacs.rutgers.edu/pub/challenge/graph/solvers/
*/

unsigned mask[INT] =
{
   1, 1<<31, 1<<30, 1<<29, 1<<28, 1<<27, 1<<26, 1<<25, 1<<24,
   1<<23, 1<<22, 1<<21, 1<<20, 1<<19, 1<<18, 1<<17, 1<<16,
   1<<15, 1<<14, 1<<13, 1<<12, 1<<11, 1<<10, 1<<9, 1<<8,
   1<<7, 1<<6, 1<<5, 1<<4, 1<<3, 1<<2, 1<<1
};  /* CAUTION - assumes 32 bit machine */

/* graph input parameters */

//int Nr_vert, Nr_edges;
BOOL Bitmap[MAX_NR_VERTICES][MAX_NR_VERTICESdiv8];
static char Preamble[MAX_PREAMBLE];
char masks[ 8 ] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

unsigned char bitmap[NMAX/CHARBITS+1][NMAX+1];
int N;         /* number of vertices in graph */

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
}

/*
   Assim que um Vertice é colocado na resposta, seus adjacentes NAO podem estar na resposta
   por isso sao invalidados.
*/
void invalidaAdjacentes(Formiga *formigaAtual, int vertice){
   int i;
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

/*
   Retorna a quantidade de Feromonio do Vertice (v)
*/
double calculaFeromonio(int vertice){
   int indice = vertice - 1;
   return vetorFeromonio[indice];
}


/*
   Retorna o valor da Heuristica para um vertice (v)
   para cada vertice (u) que nao tem aresta com v
   Heur(v)++
*/
double calculaHeuristica(Formiga *formigaAtual, int vertice){
   int i;
   int tamanho = 0;
   for(i = 1; i<= Nr_vert; i++){
      int indice_i = i - 1;
      if(formigaAtual->listaVertice[indice_i] == 0){
         if(!edge(vertice, i) && vertice != i){
            tamanho++;
         }
         
      }
   }
   return (double) tamanho;
}

/*
   A partir do Feromonio Calculado e da Heuristica, é obtido o valor da fun-prob(v)
*/
double calculaFuncao(Formiga *formigaAtual, int vertice){
   double resultado = 0.0;
   double resultadoFeromonio = calculaFeromonio(vertice);
   double resultadoHeuristica = calculaHeuristica(formigaAtual, vertice);
   
   resultadoFeromonio = pow(resultadoFeromonio, alpha);
   resultadoHeuristica = pow(resultadoHeuristica, beta);
   
   resultado = resultadoFeromonio + resultadoHeuristica;
   return resultado;
}

/*
   Escolhe o Vertice de melhor func-prob
   Caso empate, é feito um sorteio entre os empatados
*/
int escolheVertice(Formiga *formigaAtual){
   //printf("antes do escolhe");
   int i;
   int indice;
   int VerticeEscolhido = -1;
   double valorMAX = 0.0;
   int tam = 0;
   int vetorIgual[Nr_vert];
   for(i = 1; i <= Nr_vert; i++){
      indice = i - 1;
      if(formigaAtual->listaVertice[indice] == 0){
         double valor = calculaFuncao(formigaAtual, i);
         if(valor > valorMAX){
            vetorIgual[0] = i;
            tam = 1;
            valorMAX = valor;
            VerticeEscolhido = i;
         } else {
            if(valor == valorMAX){
               vetorIgual[tam] = i;
               tam++;
            }
         }
      }
   }
   //Random entre os vertices de melhor func-prob
   int indiceEscolhido = rand() %tam;
   VerticeEscolhido = vetorIgual[indiceEscolhido];
   //printf("depois do escolhe\n\n");
   return VerticeEscolhido;
}
/*
   Constroi a solucao
   1º Vertice é random
   Restante é pela funcao escolheVertice
*/
void construirSolucao(Formiga *formigaAtual){
   int numeroRandom = rand() %Nr_vert; // 0 a Nr_Vertices-1
   int numeroReal = numeroRandom + 1; //Vertice Real Ex: Random = 20, Vertice = 21
   atualizaFormiga(formigaAtual, numeroRandom); //Coloca 1 no indice do vertice, no caso [20]
   invalidaAdjacentes(formigaAtual, numeroReal);
   while(formigaAtual->verticeRestantes != 0){
      int vertice = escolheVertice(formigaAtual);
      if(vertice == -1){
         return;
      }
      int indice = vertice - 1;
      atualizaFormiga(formigaAtual, indice);
      invalidaAdjacentes(formigaAtual, vertice);
   }
}

/*
   A Resposta deve respeitar 2 principios:

   1) Principio do Conjunto Independente: Nao pode ter vertices com arestas entre si

   2) Maximo Conjunto Independente: Entre os vertices que nao estao na resposta, nao pode haver vertice que poderia estar na resposta
                            ja que estamos calculando o MAXIMO conjunto independente
*/
void verificaResposta(Formiga *formigaAtual){
   int i, j;
   int fim = formigaAtual->qtdVertice;
   //Confere Principio 1)
   for(i = 0; i < fim; i++){
      for(j = i + 1; j < fim; j++){
         if(edge(formigaAtual->vetorResposta[i], formigaAtual->vetorResposta[j])){
            printf("Erro: Nao foi formado um conjunto independente! \nOs Vertices %d -- %d sao adjacentes!\n", formigaAtual->vetorResposta[i], formigaAtual->vetorResposta[j]);
            return;
         }
      }
   }

   //Confere Principio 2)
   int soma = 0;
   for(i = 1; i <= Nr_vert; i++){
      soma = 0;
      for(j = 0; j <= fim; j++){
         int vert = formigaAtual->vetorResposta[j];
         if(i == formigaAtual->vetorResposta[j]){
            break;
         }
         if(edge(i, vert)){
            break;
         } else {
            soma++;
         }
         if(soma == fim){
            printf("Erro: Nao foi achado o maior conjunto independente! \nO Vertice %d tambem deveria estar na resposta!\n", i);
         }
         
      }
   }
}

/*
   ACO
*/
void AntSystemColony(){
   //int c;
   //printf("WRANK => %d\n", wrank);
	//printf("CICLOS => %d\n", ciclos);
	//printf("FORMIGAS => %d\n", NumeroFormigas);
	//int threadId = *(int *) args;
	
	int formiga_thread = (int) NumeroFormigas / wsize;
	//printf("FORMIGAS POR PROCESSO => %d\n", formiga_thread);

	Formiga listaFormiga[formiga_thread];
 
   int RESPOSTA[Nr_vert];
   int vertMaior, indMaior;   

   for(c = 0; c < ciclos; c++){
      //printf("\nWrank: %d -- Ciclo: %d\n", wrank, c);
      //printf("\nCiclo: %d\n", c);
		inicializarAlgoritmo(listaFormiga, formiga_thread);
   	srand (time(0)+clock()+random());
   	int i, a;
		for(i = 0; i < formiga_thread; i++){
     	   //printf("Formiga: %d\n", i);
     	   construirSolucao(&listaFormiga[i]);
     	   verificaResposta(&listaFormiga[i]);      
   	}
  
   	melhor_colonia[c] = selecionaFormiga(listaFormiga, formiga_thread);
      copiaVetor(RESPOSTA, melhor_colonia[c].vetorResposta, melhor_colonia[c].qtdVertice);
      if(wrank == 0){
         int indice, vert;
         indMaior = 0;
         vertMaior = melhor_colonia[c].qtdVertice;
         for(indice = 1; indice < wsize; indice++){
            MPI_Recv(&vert, 1, MPI_INT, indice, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(vert > vertMaior){
               indMaior = indice;
               vertMaior = vert;
            } 
         }
         MPI_Bcast(&indMaior, wsize, MPI_INT, 0, MPI_COMM_WORLD);
         MPI_Bcast(&vertMaior, wsize, MPI_INT, 0, MPI_COMM_WORLD);
      } else {
         MPI_Send(&melhor_colonia[c].qtdVertice, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
         MPI_Bcast(&indMaior, wsize, MPI_INT, 0, MPI_COMM_WORLD);
         MPI_Bcast(&vertMaior, wsize, MPI_INT, 0, MPI_COMM_WORLD);;
      }  

      if(indMaior == wrank){
         for(a = 0; a < wsize; a++){
            if(a != wrank){
               MPI_Send(&RESPOSTA, vertMaior, MPI_INT, a, 0, MPI_COMM_WORLD);
            }
         }
      } else{
         MPI_Recv(&RESPOSTA, vertMaior, MPI_INT, indMaior, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      }     

      MPI_Barrier(MPI_COMM_WORLD);

      if(vertMaior > melhorRespostaVert){
         melhorRespostaVert = vertMaior;
      }
      atualizaFeromonioComVetor(RESPOSTA, vertMaior);
      
	}
}

/*
   Funcao para mostrar as Arestas
*/
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




void ajuda(){
   printf("O programa se destina a resolver o problema do Máximo Conjunto Independente utilizado Ant System Colony.\n\n");
   printf("O programa se divide entre o aco.c (ou acoParalelo.c) e formiga.h\n\n");
   printf("A seguir os parâmetros que poderao ser atribuidos e seus valores padroes caso nao forem:\n\n");
   printf("-h: Seção Ajuda\n\n");
   printf("-i: Nome do Arquivo. \nOBRIGATORIO! \n\nAs Instancias estao na pasta Instancias. Basta copiar a instancia desejada para a mesma pasta do codigo. \nAs instancias keller estao na mesma pasta do codigo. \nSao ao todo 68 instancias diferentes.\n\n");
   printf("-a: Valor de alpha. \nPadrão: 0.5\n\n");
   printf("-b: Valor de betha. \nPadrão: 0.5\n\n");
   printf("-c: Número de ciclos. \nPadrão: 10\n\n");
   printf("-f: Número de formigas. \nPadrão: 10\n\n");
   printf("-r: Valor de rho. \nPadrão: 0.1\n\n");
   printf("-t: Número de Threads. \nPadrão: 2\n\n");
}


int main(int argc, char *argv[]){
	char nome_arquivo[60];
	int opcao;
	if(argc == 1){
	  printf("É necessário indicar ao menos o nome do arquivo de entrada!\n\n");
	  printf("Para mais detalhe, indique a diretiva -h para acessar a Seção Ajuda\n\n");
	  return 0;
	}

	while((opcao = getopt(argc,argv,"ha:b:c:f:i:r:t:")) != -1){
	  	switch (opcao) {
     	case 'h':
	    {
			ajuda();
	        return 0;
	    }
     	case 'i':
    	{
        	strcpy(nome_arquivo, optarg);
	        break;
     	}
    	case 'a':
    	{
			alpha = atof(optarg);
	        break;
	    }
	    case 'b':
	    {
    	    beta = atof(optarg);
        	break;
     	}
    	case 'c':
	    {
    	    ciclos = atoi(optarg);
	        break;
	    }
	     case 'f':
	    {
	        NumeroFormigas = atoi(optarg);
	        break;
	    }
	     case 'r':
	    {
	        rho = atof(optarg);
	        break;
	    }
	     case 't':
	    {
	        num_threads = atoi(optarg);
	        break;
	    }
	  }
	  
	}

	FILE* fp;
	if((fp = fopen(nome_arquivo, "r")) == NULL){
	  printf("Arquivo inexistente\n");
	  return 0;
	}
	readgraph(fp);

	MPI_Init(&argc, &argv);


	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);
	MPI_Comm_size(MPI_COMM_WORLD, &wsize);


	inicializarVertices();
	melhor_geral.qtdVertice = 0;
   melhor_colonia = (Formiga *) calloc (ciclos, sizeof (Formiga));
	AntSystemColony();

   if(wrank == 0){
      //printf("aaaaaaaa\n");
      //mostraRespostaColonia(&melhor_geral);
      printf("Nº Vertices => %d\n", melhorRespostaVert);
   }
   return 0;
}
