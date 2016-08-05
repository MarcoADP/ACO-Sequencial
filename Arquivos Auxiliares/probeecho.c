#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

void criarMatriz(int A[10][10], int tamanho){
	int i, j;
	for(i = 0; i < tamanho; i++){
		for(j = 0; j < tamanho; j++){
			A[i][j] = i * j;
		}
	}
}

void mostraMatriz(int A[10][10], int l, int c){
	int i, j;
	for(i = 0; i < l; i++){
		printf("\n");
		for(j = 0; j < c; j++){
			printf(" %2d ", A[i][j]);
		}
	}
	printf("\n");
}



int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);

	int wsize, wrank;
	int n = 10;	
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);
	MPI_Comm_size(MPI_COMM_WORLD, &wsize);

	int fator = (int) n / wsize; 	
	//printf("Fator => %d\n", fator);
	int filho1 = 2 * wrank + 1;
	int filho2 = 2 * (wrank + 1);
	int pai;
	if(wrank % 2 == 0){
		pai = wrank / 2 - 1;
	} else {
		pai = wrank / 2;
	}
	printf("wrank => %d -- pai => %d\n", wrank, pai);

	if(wrank == 0){
		int A[n][n];
		int B[n][n];
		int C[n][n];
		criarMatriz(A, n);
		//mostraMatriz(A, n, n);
		int restante = n - fator;
		int metade = restante / 2;
		MPI_Send(&A[fator], metade*n, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Send(&A[fator + metade], metade*n, MPI_INT, 2, 0, MPI_COMM_WORLD);
	}

	int a[n][n];
	

	if(wrank == 1 || wrank == 2){
		MPI_Recv(&a, n*n, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		///mostraMatriz(a, 4, n);
	
	}
	
	MPI_Finalize();
	return 0;
}