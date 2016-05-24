#include <stdio.h>
#include <stdlib.h>



typedef struct{
	int qtdVertice;
	int qtdVerticeIndisponiveis;
	int verticeRestantes;
	int * listaVertice;
} Formiga;

int Nr_vert, Nr_edges;
int *vetorResposta;
double *vetorVertice;
int numVerticeResposta = 0;
int NumeroFormigas = 10;
Formiga *listaFormiga;
int ciclos = 10;
double alpha = 0.5;
double beta = 0.5;
int tamanhoFormiga;

double rho = 0.1; //taxa evaporação
int num_threads = 1;

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


int selecionaFormiga(int **vetor, int c){
   int maior = listaFormiga[0].qtdVertice;
   Formiga formigaMaior;
   formigaMaior = listaFormiga[0];
   vetor[c] = (int *) calloc (Nr_vert, sizeof(int));
   int i;
   for(i = 1; i < NumeroFormigas; i++){
      if(listaFormiga[i].qtdVertice > maior){
         maior = listaFormiga[i].qtdVertice;
         formigaMaior = listaFormiga[i];
      }
   }
   int k = 0;
   for(i = 0; i < Nr_vert; i++){
      if(formigaMaior.listaVertice[i] == 1){   
         int vertice = i + 1;
         vetor[c][k] = vertice;
         k++;
      }
   }
   
   return maior;
}



void mostraRespostaColonia(int **vetor, int c, int numero){
   int i;
   printf("Melhor Resposta do Ciclo %d:\n", c);
   printf("Nº Vertices => %d\n", numero);
   printf("Vertices => ");
   for(i = 0; i < numero; i++){
      printf(" %d ", vetor[c][i]);
   }
   printf("\n\n");
}

void mostraRespostaFinal(int **vetor, int vetorIndice[], int indice){
   int i;
   int numero = vetorIndice[indice];
   printf("Melhor Resposta Final:\n");
   printf("Melhor Ciclo: %d\n", indice);
   printf("Nº Vertices => %d\n", numero);
   printf("Vertices => ");
   for(i = 0; i < numero; i++){
      printf(" %d ", vetor[indice][i]);
      //}
   }
   printf("\n\n");
}

void selecionaFormigaGlobal(int **vetor, int vetorIndice[]){
   int i;
   int maior = vetorIndice[0];
   int indice = 0;
   
   for(i = 1; i < ciclos; i++){
      if(vetorIndice[i] > maior){
         maior = vetorIndice[i];
         indice = i;
      }
   }
   mostraRespostaFinal(vetor, vetorIndice, indice);
}