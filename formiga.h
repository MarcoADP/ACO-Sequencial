#include <stdio.h>
#include <stdlib.h>



typedef struct{
	int qtdVertice;
	int qtdVerticeIndisponiveis;
	int verticeRestantes;
	int * listaVertice;
   int *vetorResposta;
} Formiga;

int Nr_vert, Nr_edges;
//int *vetorResposta;
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
int selecionaFormiga(Formiga listaFormiga[], int **vetor, int c){
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

Formiga selecionaFormigaP(Formiga listaFormiga[], int num_formiga){
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

/*
   mostra melhor formiga da colonia
*/
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

void mostraRespostaColoniaP(Formiga* formiga){
   int i, j;
   int vert = formiga->qtdVertice;
   printf("Nº Vertices => %d\n", vert);
   printf("Vertices => ");
   for(i = 0; i < Nr_vert; i++){
      if(formiga->listaVertice[i] == 1){
         j = i + 1;
         printf(" %d ", j);
      }
   }
   printf("\n\n");
}

/*
   mostra melhor resposta total
*/
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

/*
   seleciona a melhor formiga no global
*/
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