#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <limits.h>

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
#define FEROMONIO_INICIAL 30


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

void inicializarVertices(){
   vetorVertice = (double *) calloc(Nr_vert, sizeof(double));
   int i;
   for(i = 0; i < Nr_vert; i++){
      vetorVertice[i] = FEROMONIO_INICIAL;
   }
}

void inicializarAlgoritmo(){
   int i;
   vetorResposta = (int *) calloc(Nr_vert, sizeof(int));
   listaFormiga = (Formiga *) calloc(NumeroFormigas, sizeof(Formiga));
   for(i = 0; i < NumeroFormigas; i++){
      listaFormiga[i].qtdVertice = 0;
      listaFormiga[i].qtdVerticeIndisponiveis = 0;
      listaFormiga[i].verticeRestantes = Nr_vert;
      listaFormiga[i].listaVertice = (int *) calloc(Nr_vert, sizeof(int));
   }
}

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

double calculaFeromonio(int vertice){
   int indice = vertice - 1;
   return vetorVertice[indice];
}

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

   int indiceEscolhido = rand() %tam;
   VerticeEscolhido = vetorIgual[indiceEscolhido];

   return VerticeEscolhido;
}

void construirSolucao(Formiga *formigaAtual){
   
   int numeroRandom = rand() %Nr_vert; // 0 a Nr_Vertices-1
   int numeroReal = numeroRandom + 1; //Vertice Real Ex: Random = 20, Vertice = 21
   //printf("NumeroRandom => %d --  NumeroReal => %d\n", numeroRandom, numeroReal);
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

void verificaResposta(Formiga *formigaAtual){
   int i, j;
   int fim = formigaAtual->qtdVertice;
   for(i = 0; i < fim; i++){
      for(j = i + 1; j < fim; j++){
         if(edge(vetorResposta[i], vetorResposta[j])){
            printf("Erro: Nao foi formado um conjunto independente! \nOs Vertices %d -- %d sao adjacentes!\n", vetorResposta[i], vetorResposta[j]);
            return;
         }
      }
   }
   int soma = 0;
   for(i = 1; i <= Nr_vert; i++){
      soma = 0;
      for(j = 0; j <= fim; j++){
         int vert = vetorResposta[j];
         if(i == vetorResposta[j]){
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

void atualizaFeromonio(int **vetor, int valor, int c){
   int i;
   double taxa_feromonio = 1 + (2 * rho) ;
   
   for(i = 0; i < Nr_vert; i++){
      vetorVertice[i] = vetorVertice[i] * (1 - rho);   
   }
 
   for(i = 0; i < valor; i++){
      int indice = vetor[c][i] - 1;
      vetorVertice[indice] = vetorVertice[indice] *  taxa_feromonio;
   } 
}

void mostraFeromonio(){
   int i;
   for(i = 0; i < Nr_vert; i++){
      printf("Vertice %d => %lf\n", (i + 1), vetorVertice[i]);
   }
   printf("\n\n");
}

void AntSystemColony(){
   int i, c;
   inicializarVertices();
   int **resposta;
   int indiceMaior[ciclos];
   resposta = (int **) calloc (ciclos, sizeof (int *));
   
   for(i = 0; i < ciclos; i++){
      resposta[i] = (int *) calloc (Nr_vert, sizeof(int));
   }
   
   for(c = 0; c < ciclos; c++){
      //printf("\n\nCiclo: %d\n", c);
      inicializarAlgoritmo();
      srand (time(0)+clock()+random());
      for(i = 0; i < NumeroFormigas; i++){
         //printf("\n\nFormiga: %d\n", i);
         construirSolucao(&listaFormiga[i]);
         verificaResposta(&listaFormiga[i]);
         
      }
      indiceMaior[c] = selecionaFormiga(resposta, c);
      //mostraRespostaColonia(resposta, c, indiceMaior[c]);
      atualizaFeromonio(resposta, indiceMaior[c], c);
      //mostraFeromonio();
   }
   selecionaFormigaGlobal(resposta, indiceMaior);
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

void ajuda(){
   printf("O programa se destina a resolver o problema do Máximo Conjunto Independente utilizado Ant System Colony.\n\n");
   printf("A seguir os parâmetros que poderao ser atribuidos e seus valores padroes caso nao forem:\n\n");
   printf("-h: Seção Ajuda\n\n");
   printf("-i: Nome do Arquivo. \nOBRIGATORIO! \n\nAs Instancias estao na pasta Instancias. Basta copiar a instancia desejada para a mesma pasta do codigo. \nAs instancias keller estao na mesma pasta do codigo. \nSao ao todo 68 instancias diferentes.\n\n");
   printf("-a: Valor de alpha. \nPadrão: 0.5\n\n");
   printf("-b: Valor de betha. \nPadrão: 0.5\n\n");
   printf("-c: Número de ciclos. \nPadrão: 10\n\n");
   printf("-f: Número de formigas. \nPadrão: 10\n\n");
   printf("-r: Valor de rho. \nPadrão: 0.1\n\n");
   printf("-t: Número de Threads. \nPadrão: 1\n\n");
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
   AntSystemColony();
   return 0;
}
