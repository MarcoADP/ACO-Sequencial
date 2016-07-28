   #include <stdio.h>
#include <stdlib.h>


#define FEROMONIO_INICIAL 30

typedef struct{
	int qtdVertice;
	int qtdVerticeIndisponiveis;
	int verticeRestantes;
	int *listaVertice;
   int *vetorResposta;
} Formiga;

typedef struct {
   //int rank;
   int qtdVertice;
   int *vetorResposta;
} Mensagem;

int Nr_vert, Nr_edges;
double *vetorFeromonio;
int numVerticeResposta = 0;
int NumeroFormigas = 10;
//Formiga *listaFormiga;
int ciclos = 10;
double alpha = 0.5;
double beta = 0.5;
int tamanhoFormiga;

Formiga *melhor_colonia;
Formiga melhor_geral;

double rho = 0.1; //taxa evaporação
int num_threads = 2;

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
void inicializarAlgoritmo(Formiga listaFormiga[], int numero){
   int i;
   //listaFormiga = (Formiga *) calloc(numero, sizeof(Formiga)); //ADP
   for(i = 0; i < numero; i++){
      listaFormiga[i].qtdVertice = 0;
      listaFormiga[i].qtdVerticeIndisponiveis = 0;
      listaFormiga[i].verticeRestantes = Nr_vert;
      //listaFormiga[i].listaVertice = malloc(Nr_vert * sizeof(int));
      //listaFormiga[i].vetorResposta = malloc(Nr_vert * sizeof(int));
      listaFormiga[i].listaVertice = (int *) calloc(Nr_vert, sizeof(int));
      listaFormiga[i].vetorResposta = (int *) calloc(Nr_vert, sizeof(int));
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
   Seguindo a logica do ACO, eh preciso atualizar o feromonio.
   Com o passar do tempo, o feromonio vai sumindo
   Se o Vertice esta na resposta, o feromonio aumenta
*/
void atualizaFeromonio(Formiga *formiga){
   int i;
   int valor = formiga->qtdVertice;
   double taxa_feromonio = 1 + (2 * rho); 

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

void atualizaFeromonioComVetor(int *vetor, int tam){
   int i;
   int indice;
   double taxa_feromonio = 1 + (2 * rho);

   //rho = definido pelo user ou padrão 0.1
   for(i = 0; i < Nr_vert; i++){
      //printf("1 for\n");
      vetorFeromonio[i] = vetorFeromonio[i] * (1 - rho);
   }

   //taxa = 2* rho
   for(i = 1; i < tam; i++){
      //printf("%d -- %d\n", i, vetor[i]);
      //printf("2 for\n");
      //printf("%d\n", tam);
      indice = vetor[i] - 1;
      vetorFeromonio[indice] = vetorFeromonio[indice] * taxa_feromonio;

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
   Mostra os dados de Formiga
*/
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

/*
   att dados de Formiga assim que seleciona um vertice
*/
void atualizaFormiga(Formiga *formigaAtual, int indice){
   //printf("entra atualiza");
   formigaAtual->vetorResposta[formigaAtual->qtdVertice] = indice+1;
   formigaAtual->listaVertice[indice] = 1;
   formigaAtual->qtdVertice++;
   formigaAtual->verticeRestantes--;
   //printf("sai atualiza\n\n");
}

/*
   retorna o indice da formiga com a melhor resposta
*/
Formiga selecionaFormiga(Formiga listaFormiga[], int num_formiga){
   int maior = listaFormiga[0].qtdVertice;
   Formiga formigaMaior;
   formigaMaior = listaFormiga[0];
   int i;
   for(i = 1; i < num_formiga; i++){
      if(listaFormiga[i].qtdVertice > maior){
         maior = listaFormiga[i].qtdVertice;
         formigaMaior = listaFormiga[i];
      }
   }
   return formigaMaior;
}

void copiaVetor(int destino[], int origem[], int tam){
   int i;
   for(i = 0; i < tam; i++){
      destino[i] = origem[i];
   }
}

/*
   mostra melhor formiga da colonia
*/
void mostraRespostaColonia(Formiga* formiga){
   int i, j;
   int vert = formiga->qtdVertice;
   printf("\nNº Vertices => %d\n", vert);
   printf("Vertices => ");
   for(i = 0; i < Nr_vert; i++){
      if(formiga->listaVertice[i] == 1 ){
         j = i + 1;
         printf(" %d ", j);
      }
   }
   printf("\n");
}

void mostraVetor(int vetor[], int tam){
   int i;
   printf("\nVETOR =>    %d\n", tam);
   for(i = 0; i < tam; i++){
      printf(" %d ", vetor[i]);
   }
   printf("\n");
}

void mostraMensagem(Mensagem *mensagem){
   int i, tam = mensagem->qtdVertice;
   printf("\nNº Vertices => %d\n", tam);
   for(i = 0; i < tam; i++){
      printf(" %4d ", mensagem->vetorResposta[i]);
   }
   printf("\n");
}