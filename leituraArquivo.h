#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define CHARBITS 8
int NMAX;
int MAX_NR_VERTICES;
int MAX_NR_VERTICESdiv8;
int nVertice;
int nAresta;
static char Preamble[10000];

int get_params(){
	/* getting nVertice and nAresta from the preamble string,
			containing Dimacs format "p ??? num num" */

	char c, *tmp;
	char * pp = Preamble;
	int stop = 0;
	tmp = (char *)calloc(100, sizeof(char));
	
	nVertice = nAresta = 0;
	
	while (!stop && (c = *pp++) != '\0'){
		switch (c)
		  {
			case 'c':
			  while ((c = *pp++) != '\n' && c != '\0');
			  break;
			  
			case 'p':
			  sscanf(pp, "%s %d %d\n", tmp, &nVertice, &nAresta);
			  stop = 1;
			  break;
			  
			default:
			  break;
		  }
	}
	
	free(tmp);
	
	if (nVertice == 0 || nAresta == 0)
	  return 0;  /* error */
	else
	  return 1;
	
}

void leituraArquivo(char nome_arquivo[]){
	FILE *fp;
	int length;
	int N;

	if((fp = fopen(nome_arquivo, "r")) == NULL){
		printf("Arquivo inexistente\n");
		return;
	}
	//printf("Arquivo existe!\n");


	if (!fscanf(fp, "%d\n", &length)){ 
		printf("ERROR: Corrupted preamble.\n"); exit(10); 
	}
	fread(Preamble, 1, length, fp);
	Preamble[length] = '\0';

	if(!get_params()){
		printf("Header Corrompido\n");
		return;
	}
	NMAX = nVertice;
	MAX_NR_VERTICES = nVertice;
	MAX_NR_VERTICESdiv8 = MAX_NR_VERTICES / 8;
	char Bitmap[MAX_NR_VERTICES][MAX_NR_VERTICESdiv8];
	unsigned char bitmap[NMAX/CHARBITS+1][NMAX+1];

	int i, j;

	for ( i = 0; i < nVertice && fread(Bitmap[i], 1, (int)((i + 8)/8), fp); i++ );

	fclose(fp);

	N = nVertice;

	for (i = 0; i < N; i++){
   		for (j = 0; j < N; j++){
   			//if (get_edge(i, j)){
       			bitmap[(j+1)/CHARBITS][i+1] |= (1 << ((j+1) % CHARBITS));
   			//}
   		}
	}
	

}