#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
 
#define MAX_THREADS 2
#define N 6
#define FATOR 1
#define ITER 3

int A[N][N];
int B[N][N];
int qtd = 0;

pthread_cond_t condVar; 
pthread_mutex_t monitorLock;

typedef struct{
    int id;
    int iter;
} thread_arg;

 
void preenche(int A[N][N], int m, int n){ // Preenche matriz com '1'
 	int i,j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			A[i][j] = i + j + i*j;
}

void imprime(int A[N][N], int m, int n){ // Exibe a matriz na tela
	int i,j;
	for (i = 0; i < m; i++){
 		for (j = 0; j < n; j++){
 			printf("|%d|\t", A[i][j]);
 		}
 		printf("\n");
 	}
		printf("-----------------------------------------------------\n");
}

void calculaSEQ(int X[N][N], int Y[N][N], int m, int n){
	int i, j;
	for(i = 0; i < m; i++){
		for(j = 0; j <n; j++){
			if(i == 0 || j == 0 || i == (m-1) || j == (n-1)){
				Y[i][j] = X[i][j];
			} else {
				Y[i][j] = (X[i-1][j] + X[i+1][j] + X[i][j-1] +X[i][j+1])/FATOR;
			}
		}
	}
}

int barreira(){
	pthread_mutex_lock(&monitorLock);
	qtd++;
	
	if(qtd < MAX_THREADS){
		printf("CALMA %d\n", qtd);
		pthread_cond_wait(&condVar, &monitorLock);
	} else {
		printf("ACABOU %d\n", qtd);
		pthread_cond_broadcast(&condVar);
		qtd = 0;
	}
	pthread_mutex_unlock(&monitorLock);
	return 0;
}

//void *calculaPAR(void *id){
void *calculaPAR(void *vargp){
	thread_arg *arg = (thread_arg *) vargp;
	int threadId = arg->id;
	int iter = arg-> iter;
	//int threadId = (int)id;
	//printf("threadID: %d\n", threadId);
	int i,j;//,k;
	//int X[N][N];
	//int Y[N][N];
	//printf("ID: %d\n", threadId);
	for (i = (threadId*(N/MAX_THREADS)); i < ((threadId+1)*(N/MAX_THREADS)); i++){
		for (j = 0; j < N; j++){
			//printf("INDICES: %d -- %d\n", i, j);
			if(iter % 2 == 0){
				if(j == 0 || i == 0 || i == (N - 1) || j == (N - 1)){
					B[i][j] = A[i][j];
				} else {
					B[i][j] = (A[i-1][j] + A[i+1][j] + A[i][j-1] + A[i][j+1])/FATOR;
				}
			} else {
				if(j == 0 || i == 0 || i == (N - 1) || j == (N - 1)){
					A[i][j] = B[i][j];
				} else {
					A[i][j] = (B[i-1][j] + B[i+1][j] + B[i][j-1] + B[i][j+1])/FATOR;
				}
			}
	 	}
	 	barreira();
		
 	}


	/*if(iter % 2 == 0){
		for (i = (threadId*(N/MAX_THREADS)); i < ((threadId+1)*(N/MAX_THREADS)); i++){
			for (j = 0; j < N; j++){
				//printf("INDICES: %d -- %d\n", i, j);
				if(j == 0 || i == 0 || i == (N - 1) || j == (N - 1)){
					B[i][j] = A[i][j];
				} else {
					B[i][j] = (A[i-1][j] + A[i+1][j] + A[i][j-1] + A[i][j+1])/FATOR;
				}
		 	}
			
	 	}
 	} else {
 		for (i = (threadId*(N/MAX_THREADS)); i < ((threadId+1)*(N/MAX_THREADS)); i++){
			for (j = 0; j < N; j++){
				//printf("INDICES: %d -- %d\n", i, j);
				if(j == 0 || i == 0 || i == (N - 1) || j == (N - 1)){
					A[i][j] = B[i][j];
				} else {
					A[i][j] = (B[i-1][j] + B[i+1][j] + B[i][j-1] + B[i][j+1])/FATOR;
				}
		 	}
		}
 	}*/
 	pthread_exit(NULL);
} 
 
int main(int argc, char **argv) {
	// Declaraçã o dos parâmetros do pthread
	pthread_t threads[MAX_THREADS];
	pthread_attr_t atributos;
	pthread_attr_init(&atributos);
	pthread_attr_setdetachstate(&atributos, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setscope(&atributos,PTHREAD_SCOPE_SYSTEM);
	//pthread_barrier_t our_barrier;
	pthread_mutex_init(&monitorLock, NULL);
	pthread_cond_init(&condVar, NULL);
	int a, i,flag;
	preenche(A,N,N); //Preenche toda a matriz A[N][N] com '1'
	//preenche(B,N,N); //Preenche toda a matriz B[N][N] com '1'
	//calculaSEQ(A, B, N, N);
 	// Criaçã o das threads. Se houver erro de criaçã o, o programa para de executar.
	for (a = 0; a < ITER; a++){
		printf("ITERACAO => %d\n\n", a);
		for (i = 0; i < MAX_THREADS; i++){
			//pthread_barrier_init(&our_barrier,NULL,4);
			thread_arg arg[MAX_THREADS];
		
			arg[i].id = i;
			arg[i].iter = a;
			//printf("ID %d\n", arg[i].id);
			//flag = pthread_create(&threads[i], &atributos, calculaPAR, (void *) i);
			flag = pthread_create(&threads[i], &atributos, calculaPAR, (void *) &arg[i]);
		
		}
		if (flag)
				exit(-1);
			
		// Junçã o dos resultados obtidos por cada thread
		for (i = 0; i < MAX_THREADS; i++)
			pthread_join(threads[i], NULL);

		//pthread_barrier_destroy(&our_barrier);
		//Imprime as matrizes na tela
		imprime(A,N,N);
		imprime(B,N,N);
	}
	pthread_mutex_destroy(&monitorLock);
	pthread_cond_destroy(&condVar);
	pthread_exit(NULL);

}
  
