#include <stdio.h>
#include <stdlib.h>

int tamanhoFormiga;

/*typedef struct{
	int id;
} dadosFormiga;*/

typedef struct{
	int qtdVertice;
	int qtdVerticeIndisponiveis;
	int verticeRestantes;
	int * listaVertice;
} Formiga;


/*

//OPERACOES DE LISTA FORMIGA
void inicialista(formiga *lista){
	lista->proximaFormiga = NULL;
	tamanhoFormiga = 0;
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
}*/