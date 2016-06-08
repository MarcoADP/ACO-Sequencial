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

//Ver como paralelizar
//Quando tem um vetor global, quando da malloc em cada thread, ele vira local para a thread?
//Vetor Feromonio é global, logo precisa inicializar uma unica vez?
//Atualizar Feromonio é área compartilhada, logo precisa de lock?
//Quando printa o Id zoa, tipo dois 2 e nenhum 1

int TESTE = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
//pthread_barrier_t barreira = PTHREAD_COND_INITIALIZER;
pthread_barrier_t barreira;

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
   Aloca Dinamicamente o Vetor de Feromonio
   |Vetor de Feromonio| = Numero de Vertices
   Cada posicao inicia com um valor Inicial -> 30

*/
void inicializarVertices(){
   vetorFeromonio = (double *) calloc(Nr_vert, sizeof(double));
   int i;
   for(i = 0; i < Nr_vert; i++){
      vetorFeromonio[i] = FEROMONIO_INICIAL;
   }
}

/*
   Inicializa o vetorResposta -> vetor que armazena a resposta
   listaFormiga -> vetor de Formigas
   Formiga armazena: (esta definida em formiga.h)
      Quantidade Vertices na Resposta
      Quantidade de Vertices que NÃO podem estar na Resposta
      Quantidade de Vertices que AINDA não foram analisados
      OBS: A soma deve ser igual ao numero de Vertices
      Lista de Vertices: 
         se o Vertice x esta na resposta, a posicao (x-1) = +1
         se o vertice x nao esta na resp, a posicao (x-1) = -1
         se o vertice X nao foi analisado, a posicao (x-1) = 0
*/
void inicializarAlgoritmo(int numero){
   int i;
   listaFormiga = (Formiga *) calloc(numero, sizeof(Formiga)); //ADP
   for(i = 0; i < NumeroFormigas; i++){
      listaFormiga[i].qtdVertice = 0;
      listaFormiga[i].qtdVerticeIndisponiveis = 0;
      listaFormiga[i].verticeRestantes = Nr_vert;
      listaFormiga[i].listaVertice = (int *) calloc(Nr_vert, sizeof(int));
      listaFormiga[i].vetorResposta = (int *) calloc(Nr_vert, sizeof(int));
   }
}

/*
   Assim que um Vertice é colocado na resposta, seus adjacentes NAO podem estar na resposta
   por isso sao invalidados.
*/
void invalidaAdjacentes(Formiga *formigaAtual, int vertice){
   //printf("entra invalida ");
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
  // printf("sai invalida\n\n");
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
   //printf("NumeroRandom => %d --  NumeroReal => %d\n", numeroRandom, numeroReal);
   atualizaFormiga(formigaAtual, numeroRandom); //Coloca 1 no indice do vertice, no caso [20]
   invalidaAdjacentes(formigaAtual, numeroReal);
   //printf("Antes do while do construir\n\n");
   while(formigaAtual->verticeRestantes != 0){
      //printf("dentro do while\n");
      int vertice = escolheVertice(formigaAtual);
      if(vertice == -1){
         return;
      }
      int indice = vertice - 1;
      atualizaFormiga(formigaAtual, indice);
      //printf("depois do atualiza!\n\n");
      invalidaAdjacentes(formigaAtual, vertice);
      //printf("depois do invalida!!!!\n");
   }
   //printf("fora do while!!\n\n");
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
   Seguindo a logica do ACO, eh preciso atualizar o feromonio.
   Com o passar do tempo, o feromonio vai sumindo
   Se o Vertice esta na resposta, o feromonio aumenta
*/
void atualizaFeromonio(int **vetor, int valor, int c){
   int i;
   double taxa_feromonio = 1 + (2 * rho) ;

   //rho = definido pelo user ou padrão 0.1

   for(i = 0; i < Nr_vert; i++){
     vetorFeromonio[i] = vetorFeromonio[i] * (1 - rho);   
   }

   //taxa - 2 * rho
   for(i = 0; i < valor; i++){
     int indice = vetor[c][i] - 1;
     vetorFeromonio[indice] = vetorFeromonio[indice] *  taxa_feromonio;
   } 
}

void atualizaFeromonioP(Formiga *formiga){
   int i;
   double taxa_feromonio = 1 + (2 * rho) ;
   int valor = formiga->qtdVertice;

   //rho = definido pelo user ou padrão 0.1

   for(i = 0; i < Nr_vert; i++){
      vetorFeromonio[i] = vetorFeromonio[i] * (1 - rho);   
   }

   //taxa - 2 * rho
   for(i = 0; i < valor; i++){
      int indice = formiga->listaVertice[i] - 1;
      vetorFeromonio[indice] = vetorFeromonio[indice] *  taxa_feromonio;
   } 
}


/*
   Funcao auxiliar para mostrar o feromonio em cada vertice
*/
void mostraFeromonio(){
   int i;
   for(i = 0; i < Nr_vert; i++){
      printf("Vertice %d => %lf\n", (i + 1), vetorFeromonio[i]);
   }
   printf("\n\n");
}
/*
   ACO
*/
void *AntSystemColony(void *args){
   int c;
   //thread_arg *arg = (thread_arg *) vargp;
   int threadId = *(int *) args;
   
   int formiga_thread = (int) NumeroFormigas / num_threads;
   melhor_colonia = (Formiga *) calloc (ciclos, sizeof (Formiga *));
   //printf("Antes dos ciclos\n");
   //printf("ID => %d\n\n", threadId);

   //pthread_t self;
   //self = pthread_self();
   //threadId = (int) self;

   for(c = 0; c < ciclos; c++){
      //TESTE++;
      //printf("Teste => %d\n", ++TESTE);
      //printf("Thread: %d\n", threadId);
      printf("\nThread: %d\nCiclo: %d\n", threadId, c);
      //inicializarAlgoritmo(formiga_thread);
      srand (time(0)+clock()+random());
      int i;
      for(i = 0; i < formiga_thread; i++){
         //printf("Formiga: %d\n", i);
         //printf("oi\n\n");
         //construirSolucao(&listaFormiga[i]);
         /*//printf("oiiii\n\n");
         verificaResposta(&listaFormiga[i]);*/
         
      }
      /*pthread_mutex_lock(&lock);
      melhor_colonia[c] = selecionaFormigaP( threadId, formiga_thread);
      if(melhor_colonia[c].qtdVertice > melhor_geral.qtdVertice){
         melhor_geral = melhor_colonia[c];
      }
      //indiceMaior[c] = selecionaFormiga(resposta, c);
      //mostraRespostaColonia(resposta, c, indiceMaior[c]);
      //atualizaFeromonio(resposta, indiceMaior[c], c);
      //mostraFeromonio();

      pthread_mutex_unlock(&lock);


      atualizaFeromonioP(&melhor_colonia[c]);
      mostraRespostaColoniaP(&melhor_colonia[c]);*/

      pthread_barrier_wait(&barreira);
   }
   //selecionaFormigaGlobal(resposta, indiceMaior);
   int *ia = malloc(sizeof(int));
   return ia;
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

   //==================
   pthread_mutex_init(&lock, NULL);
   pthread_barrier_init(&barreira, NULL, num_threads);
   pthread_t threads[num_threads];

   //==================
   inicializarVertices();
   int i;
   for(i = 0; i < num_threads; i++){
      //thread_arg arg[num_threads];
      
      //arg[i].id = i;
      printf("i -> %d\n\n", i);
      //printf("i -> %d -- id -> %d\n\n", i, arg[i].id);
      pthread_create(&threads[i], NULL, AntSystemColony, (void *) &i);
   }
   for(i = 0; i < num_threads; i++){
      pthread_join(threads[i], NULL);
   }
   //AntSystemColony();
   return 0;
}
