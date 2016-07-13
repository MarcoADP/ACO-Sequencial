#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#include "mpi.h"
int size;



typedef struct reg* Reg;
struct reg{
	int linha;
	int coluna;
	int valor;
	//Reg* proximo;
};

int criarMatriz(int** A){
	int i, j;
	int porcentagem = (int) size * 0.35;
	int total = 0;
	for(i = 0; i < size; i++){
		int porcentagemLinha = (int) size * 0.01;
		int totalLinha = 0;
		for(j = 0; j < size; j++){
			if(total == porcentagem || totalLinha == porcentagemLinha){
				A[i][j] = 0;
			} else {
				int numRandom = rand() % 10;
				if(numRandom < 2 && numRandom > 0){
					A[i][j] = numRandom;
					total++;
					totalLinha++;	
				} else {
					A[i][j] = 0;
				}
			}
		}
	}
	//printf("total => %d\n", total);
	return total;
}

void mostraMatriz(int **A){
	int i, j;
	for(i = 0; i < size; i++){
		printf("\n");
		for(j = 0; j < size; j++){
			printf("%d -- ", A[i][j]);
		}
	}
	printf("\n");
}

void mostraVetor(Reg vetor, int size){
	int i;
	for(i = 0; i < size; i++){
		printf("linha => %d\n", vetor[i].linha);
		printf("coluna => %d\n", vetor[i].coluna);
		printf("valor => %d\n\n", vetor[i].valor);
	}
}

int main(int argc, char *argv[]){

	//MPI_Init(&argc, &argv);


	//MPI_Comm_rank(MPI_COMM_WORLD, &wrank);
	//MPI_Comm_size(MPI_COMM_WORLD, &wsize);
	srand (time(0)+clock()+random());
	int **A, **B, **C;
	
	int i, j;

	size = 100;

	A = calloc (size, sizeof(int *));
	B = calloc (size, sizeof(int *));
	C = calloc (size, sizeof(int *));

	for(i = 0; i < size; i++){
		A[i] = calloc (size, sizeof(int));
		B[i] = calloc (size, sizeof(int));
		C[i] = calloc (size, sizeof(int));
	}

	int totalA = criarMatriz(A);
	int tamA = 0;
	int totalB = criarMatriz(B);
	int tamB = 0;
	//mostraMatriz(A);

	Reg vetorA = (Reg) calloc(totalA, sizeof(Reg));
	Reg vetorB = (Reg) calloc(totalB, sizeof(Reg));

	for(i = 0; i < size; i++){
		for(j = 0; j < size; j++){	
			if(A[i][j] != 0){
		 		vetorA[tamA].linha = i;
		 		vetorA[tamA].coluna = j;
		 		vetorA[tamA].valor = A[i][j];
		 		tamA++;
			}
			if(B[j][i] != 0){
				vetorB[tamB].linha = i;
		 		vetorB[tamB].coluna = j;
		 		vetorB[tamB].valor = B[j][i];
		 		tamB++;
			}
		}
	}

	int* tarefas = calloc(size, sizeof(int *)); 
	//mostraVetor(vetorA, tamA);


	//MPI_Finalize();
	return 0;
}