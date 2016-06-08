#include <string.h>
#include <stdio.h>
#include <stdlib.h>


struct Node{
	int count;
	struct Node *parent;
	int sense;
};

typedef struct Node node;



typedef struct {
	int radix;
	node *leaf;
	int leaves;
	int threadSense;
	//criar var local para thread threadSense
} TreeBarrier;


node* inicializaNode(int radix){
	node *no = malloc(sizeof(node));
	no->sense = 0;
	no->parent = NULL;
	no->count = radix;

	return no;
}

node* constroiNo(int radix, node *pai){
	node *no = malloc(sizeof(node));
	no = inicializaNode(radix);
	no->parent = pai;

	return no;
}

void await(TreeBarrier *barreira, node *no){
	int mySense = barreira->threadSense;
	int position = no->count;
	no->count--;
	if(position == 1){
		if(no->parent != NULL){
			await(barreira, no->parent);
		}
		no->count = barreira->radix;
		no->sense = mySense;
	} else {
		while(no->sense != mySense);
	}
	if(mySense == 0){
		barreira->threadSense = 1;	
	} else {
		barreira->threadSense = 0;
	}
	 
}

//ARVORE
void build(node *parent, int depth, TreeBarrier *barreira){
	if(depth == 0){
		//barreira->leaf[barreira->leaves++] = parent;
	} else {
		int i;
		for(i = 0; i < barreira->radix; i++){
			node* child = constroiNo(barreira->radix, parent);
			build(child, depth-1, barreira);
		}
	}
}

void awaitTree(TreeBarrier *barreira){ //arrumar
	//int me = 0; //mudar a inicialização
	//int posicao = (int) me/barreira->radix;
	//node myLeaf =  barreira->leaf[posicao];
	awaitTree(barreira);
}

void fTreeBarrier(int n, int r, TreeBarrier *barreira){
	barreira->radix = r;
	barreira->leaves = 0;
	int tamanho = (int) n/r;
	barreira->leaf = (node *) calloc(tamanho, sizeof(node));
	int i;
	for(i = 0; i < tamanho; i++){
		barreira->leaf[0] = inicializaNode(barreira->radix);
	}
	int depth = 0;
	//threadSense
	while(n > 1){
		depth++;
		n = n/r;
	}
	node *root = inicializaNode(barreira->radix); 
	build(root, depth-1, barreira);
}

int main(){
	return 0;
}