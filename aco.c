#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "formiga.h"
#include "grafo.h"
//#include "leituraArquivo.h"

//#define edge(x,y) (bitmap[y/CHARBITS][x] & (1<<(y%CHARBITS)))
#define INT 32		/* computer word size */
#define CHARBITS 8
#define edge(x,y) (bitmap[y/CHARBITS][x] & (1<<(y%CHARBITS)))

#define NMAX 3600		/* maximum number of vertices handles */
#define MAX_NR_VERTICES		3600	/* = NMAX */
#define MAX_NR_VERTICESdiv8	450	/* = NMAX/8 */
#define BOOL	char
#define MAX_PREAMBLE 10000
#define REORDER

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

int Nr_vert, Nr_edges;
BOOL Bitmap[MAX_NR_VERTICES][MAX_NR_VERTICESdiv8];
static char Preamble[MAX_PREAMBLE];
char masks[ 8 ] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

unsigned char bitmap[NMAX/CHARBITS+1][NMAX+1];
int N;			/* number of vertices in graph */

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
		
	int ii, jj, total = 0;
	for(ii = 1; ii <= Nr_vert; ii++){
		for(jj = 1; jj <= Nr_vert; jj++){
			if(edge(ii, jj) && ii > jj){
				printf("%d -- %d\n", ii, jj);
				total++;
			}
		}
	}
	printf("%d -- %d\n", total, Nr_edges);
}



int main(int argc, char *argv[]){
	char nome_arquivo[60];
	FILE* fp;
	//printf("%s\n", argv[0]);
	strcpy(nome_arquivo, argv[1]);
	if((fp = fopen(nome_arquivo, "r")) == NULL){
		printf("Arquivo inexistente\n");
		return 0;
	}
	readgraph(fp);
	/*int i, j;
	for(i = 1; i <= nVertice; i++){
	for(j = 1; j <= nVertice; j++){
	if(edge(i, j)){
	printf("%d -- %d\n", i, j);
}
}
}*/
//printf("%s\n", nome_arquivo);
//printf("%s\n", argv[2]);
return 0;
}

/*
int main(int argc, char **argv){
int opt;
printf("Iniciou o programa!\n");
if(argc < 2){
printf("Entre	 com as entradas... \n");
return 0;
}
int oi;
while( (opt = getopt(argc, argv, "hn:i:e:c:a")) > 0 ) {
switch(opt){
case 'a':
oi = (int) getopt;
break;
}
}

printf("%d\n", oi);

//vertice *vert = (vertice *) malloc(sizeof(vertice));
//vert->listaFormiga = (formiga *) malloc(sizeof(formiga));
formiga *lista = (formiga *) malloc(sizeof(formiga));
/ *if(!lista){
printf("Sem memoria disponivel!\n");
exit(1);
}else{
inicialista(vert->listaFormiga);
}

int i;
for(i = 0; i < 10; i++){
dadosFormiga novaFormiga;
novaFormiga.id = 2 * i;
insereFormigaFim(vert->listaFormiga, novaFormiga);
}

mostralista(vert->listaFormiga);

free(vert->listaFormiga); tem '* /' aqui

aresta *arestasOriginais = (aresta * ) malloc(sizeof(aresta));

leituraEntrada(arestasOriginais);
mostraAresta(arestasOriginais);
//printf("Procura 1 => %d\n", existeAresta(arestasOriginais, 0, 1));
//printf("Procura 2 => %d\n", existeAresta(arestasOriginais, 1, 0));
//printf("Procura 3 => %d\n", existeAresta(arestasOriginais, 0, 0));
//printf("Procura 4 => %d\n", existeAresta(arestasOriginais, 4, 1));
//printf("Procura 5 => %d\n", existeAresta(arestasOriginais, 1, 4));
//printf("Procura 6 => %d\n", existeAresta(arestasOriginais, 5, 4));

coloniaSystem(lista);

free(lista);
printf("Programa Encerrado!\n");
return 0;
}
*/
