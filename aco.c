#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//ESTRUTURAS
typedef struct{
	int id;
} dadosFormiga;

struct Formiga{
	dadosFormiga membro;
	struct Formiga *proximaFormiga;
};
typedef struct Formiga formiga;

int tamanhoformiga;

typedef struct{
	int id;	
	int quantidadeFormigas;
	formiga *listaFormiga;	
} vertice;

typedef struct {
	vertice origem;
	vertice destino;
	float peso;
	float feromonio;
} aresta;

//OPERACOES DE LISTA
void inicialista(formiga *lista){
	lista->proximaFormiga = NULL;
	tamanhoformiga = 0;
}

int vazia(formiga *colonia){
	if(colonia->proximaFormiga == NULL)
		return 1;
	else
		return 0;
}

formiga *aloca(dadosFormiga novaFormiga){
	formiga *novo =(formiga *) malloc(sizeof(formiga));
	if(!novo){
		printf("Sem memoria disponivel!\n");
		exit(1);
	}else{
		//printf("Novo elemento: "); scanf("%d", &novo->num);
		novo->membro = novaFormiga;
		return novo;
	}
}

void insereFormigaFim(formiga *lista, dadosFormiga novaFormiga){
	formiga *novo = aloca(novaFormiga);
	novo->proximaFormiga = NULL;
	if(vazia(lista)){
		lista->proximaFormiga = novo;
	} else {
		formiga *tmp = lista->proximaFormiga;
		while(tmp->proximaFormiga != NULL){
			tmp = tmp->proximaFormiga;
		}

		tmp->proximaFormiga = novo;
	}
}

void mostralista(formiga *lista){
	//system("clear");
	if(vazia(lista)){
		printf("lista vazia!\n\n");
		return ;
	}
	
	formiga *tmp;
	tmp = lista->proximaFormiga;
	printf("lista: \n");
	while( tmp != NULL){
		printf("ID Formiga => %d\n", tmp->membro.id);
		//printf("ID Proxima Formiga => %d\n\n", tmp->proximaFormiga->membro.id);
		tmp = tmp->proximaFormiga;
	}
	
		
	printf("\n\n");
}


//ANT COLONY OPTIMIZATION
void buscaLocal(){

}

void atualizarFeromonio(){

}

void gerarSolucoes(){

}

void inicializaAlgoritmo(){

}

void coloniaSystem(formiga *listaFormigas){
	inicializaAlgoritmo();

	gerarSolucoes();

	atualizarFeromonio();

	buscaLocal();
}

void leituraEntrada(){

}

int main(){
	
	printf("Iniciou o programa!\n");
	vertice *vert = (vertice *) malloc(sizeof(vertice));
	vert->listaFormiga = (formiga *) malloc(sizeof(formiga));
	formiga *lista = (formiga *) malloc(sizeof(formiga));
	if(!lista){
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

	free(vert->listaFormiga);
	
	
	leituraEntrada();

	coloniaSystem(lista);
	
	free(lista);
	printf("Programa Encerrado!\n");
	return 0;
}
