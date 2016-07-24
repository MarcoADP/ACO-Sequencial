#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

void criarMatriz(int** matriz, int tam){
	int i, j;
	for(i = 0; i < tam; i++){
		for(j = 0; j < tam; j++){
			matriz[i][j] = rand() %10;
		}
	}
}

void mostraMatriz(int** matriz, int linha, int coluna){
	int i, j;
	for(i = 0; i < linha; i++){
		printf("\n");
		for(j = 0; j < coluna; j++){
			printf(" %d ", matriz[i][j]);
		}
	}
	printf("\n\n");
}

int **create_board(int n, int l){
    int **new_matrix = malloc(l * sizeof(int *));
    new_matrix[0] = calloc(l * n, sizeof(int));
    for (int i = 0; i < n; i++)
        new_matrix[i] = (*new_matrix + i * n);
    return new_matrix;
}

int main(int argc, char *argv[]){
	int tamanho = 5;
	
	srand (time(0)+clock()+random());
	
	//mostraMatriz(A, tamanho);
	//mostraMatriz(B, tamanho);

	MPI_Init(&argc, &argv);
	int wrank, wsize;
	MPI_Status status;
	int tag;
	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);
	MPI_Comm_size(MPI_COMM_WORLD, &wsize);

	if(wrank == 0){
		int** A = create_board(tamanho, tamanho);
		int** B = create_board(tamanho, tamanho);

		criarMatriz(A, tamanho);
		criarMatriz(B, tamanho);
		
		//MPI_Send(&BUFFER, NUM_ELEMENTOS, MPI_INT, DESTINO, TAG, COMUNICADOR);
		MPI_Send(A[0], tamanho * tamanho, MPI_INT, 1, tag, MPI_COMM_WORLD);
		MPI_Send(B[0], tamanho * tamanho, MPI_INT, 1, tag, MPI_COMM_WORLD);
	
	} else {
		int tamanhoReal = tamanho - wrank + 1;
		int** A = create_board(tamanho, tamanhoReal);
		int** B = create_board(tamanho, tamanhoReal);

		//MPI_Recv(&BUFFER, NUM_ELEMENTOS, MPI_INT, FONTE, TAG, COMUNICADOR, STATUS, ERROR);
		MPI_Recv(A[0], tamanhoReal * tamanho, MPI_INT, wrank-1, tag, MPI_COMM_WORLD, &status);
		MPI_Recv(B[0], tamanho * tamanho, MPI_INT, wrank-1, tag, MPI_COMM_WORLD, &status);
		
		if(wrank != (wsize - 1)){
			int tamanhoSend = tamanhoReal - 1;	
			MPI_Send(A[wrank], tamanhoSend * tamanho, MPI_INT, wrank+1, tag, MPI_COMM_WORLD);
			MPI_Send(B[0], tamanho * tamanho, MPI_INT, wrank+1, tag, MPI_COMM_WORLD);
		}
		//printf("%d\n", buffer[0]);
		//mostraMatriz(A, tamanhoReal, tamanho);
		//mostraMatriz(B, tamanho, tamanho);
	}



	MPI_Finalize();

	return 0;
}