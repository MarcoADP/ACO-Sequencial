#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mpi.h"

int* vetor;

void criaMatriz(int** matriz, int tamanho){
	int numero;
	int i, j, celula;

	for(i = 0; i < tamanho; i++){
		for(j = 0; j < tamanho; j++){
			numero = rand() % 100;
			if(numero > 50){
				celula = i * tamanho + j; 
				matriz[i][j] = 1;
				vetor[celula] = 1;
			} 
		}
	}
}

void mostraMatriz(int** matriz, int tamanho){
	int i, j;
	for(i = 0; i < tamanho; i++){
		printf("\n");
		for(j = 0; j < tamanho; j++){
			printf("%d ", matriz[i][j]);
		}
	}
	printf("\n\n");
}

void mostraVetor(int* vetor, int tamanho){
	int i;
	printf("\nVETOR");
	for(i = 0; i < tamanho; i++){
		if(i % 10 == 0){
			printf("\n");
		}
		printf("%d ", vetor[i]);
	}
	printf("\n");
}

void verificavizinho(int id, int total){
	if(vetor[id] == 1){
		if(total < 2 || total > 4){
			vetor[id] = 0;
		}
	} else {
		if(total == 3){
			vetor[id] == 1;
		}
	}

}

int main(int argc, char *argv[]){
	int tamanho = 10;
	int num_celula = tamanho * tamanho;
	vetor = calloc(num_celula, sizeof(int));
	int i, j;
	srand(time(NULL));
	int wsize, wrank;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &wrank);
	MPI_Comm_size(MPI_COMM_WORLD, &wsize);

	int linha_por_processo = (int) tamanho / wsize;
	int inicio = wrank * linha_por_processo;
	int fim = inicio + linha_por_processo;
	int tamanhoBuffer = linha_por_processo * tamanho;
	int *buffer = calloc(tamanhoBuffer, sizeof(int));

	if(wrank == 0){
		int** matriz;
		matriz = calloc(tamanho, sizeof(int *));
		for(i = 0; i < tamanho; i++){
			matriz[i] = calloc(tamanho, sizeof(int));
		}

		criaMatriz(matriz, tamanho);
		mostraMatriz(matriz, tamanho);
		//mostraVetor(num_celula);
		for(i = 1; i < wsize; i++){
			//printf("%d\n", i * linha_por_processo);
			mostraVetor(buffer, tamanhoBuffer);
			memcpy(buffer, matriz[i * linha_por_processo], tamanhoBuffer * sizeof(int));
			mostraVetor(buffer, tamanhoBuffer);
		}


	}

	
	


	MPI_Finalize();
	return 0;
}