/*
 * Aluno: Vinicius Ferraço Arruda
 * Curso: Ciência da Computação
 * E-mail: viniciusferracoarruda@gmail.com
 * Disciplina: Estrutura de Dados I
 * Professor: Thomas W. Rauber
 * Data: 28 de junho de 2015
 */

#include <stdio.h>
#include <stdlib.h>
#include "VariableTree.h"


int emptyTree(Tree* tree)                                                          /* Verifica se a árvore tree está vazia. */
{
	return tree == NULL;	
}


Tree* createTree(char info)                                                        /* Cria um nó do tipo Tree e retorna seu endereço. */
{
	Tree* new = (Tree *) malloc(sizeof(Tree));

	if(new == NULL)
	{
		fprintf(stderr, "Error while allocating memory.\n");
		exit(EXIT_FAILURE);
	}
	
	new->info = info;                                                              /* Atribui o valor passado como parâmetro ao nó. */
	new->next = NULL;
	new->firstChild = NULL;
	new->lastChild = NULL;
	
	return new;
}


void insertEnd(Tree* tree, char info)                          /* Insere um novo filho no final da lista de filhos da árvore tree. */
{
	Tree* new = createTree(info);                              /* Cria um nó com a informação passada por parâmetro. */

	if(emptyTree(tree->firstChild))	                           /* Se a árvore não possui filho algum, */
		tree->firstChild = new;                                /* o nó recentemente criado se torna o primeiro filho. */
	else                                                       /* Caso contrário, */
		tree->lastChild->next = new;                           /* o último filho aponta para o nó recentemente criado. */

	tree->lastChild = new;                                     /* Atualiza o ponteiro para o último filho. */
}


void freeTree(Tree** tree)                                     /* Libera a árvore. */
{
	if(emptyTree(*tree))                                       /* Se a árvore estiver vazia, retorna. */
		return;
	
	freeTree(&(*tree)->firstChild);                            /* Libera recursivamente o primeiro filho do nó corrente. */
	freeTree(&(*tree)->next);                                  /* Libera recursivamente o irmão do nó corrente. */
	free(*tree);                                               /* Libera o nó corrente. */
	*tree = NULL;                                              /* Atribui NULL ao ponteiro do nó corrente. */
}


/*
 * Caminha pela árvore e caso o nó corrente for uma folha, chama a função de callback.
 */
void travelsTreeLeaf(Tree* tree, void (* function)(Tree *, void *), void* arg) 
{	
	if(emptyTree(tree))	                                                  /* Caso a árvore for vazia, retorna. */ 
		return;

	if(emptyTree(tree->firstChild))                                       /* Caso o filho da árvore for vazio, então é uma folha. */
		function(tree, arg);                                              /* Chama a função function passando a referência para o nó */
	else                                                                  /* corrente e arg que é um argumento qualquer (Depende da aplicação). */
		travelsTreeLeaf(tree->firstChild, function, arg);                 /* Caso contrário, chama travelsTreeLeaf recursivamente, passando */
                                                                          /* a referência para o primeiro filho. */
	travelsTreeLeaf(tree->next, function, arg); 		                  /* Chama travelsTreeLeaf recursivamente, passando a referência para */
}                                                                         /* o nó irmão do nó corrente. */


void travelsTree(Tree* tree, void (* function)(char))              /* Caminha pela árvore, chamando a função function para cada nó. */ 
{
	if(emptyTree(tree))                                            /* Caso a árvore for vazia, retorna. */
		return;

	function(tree->info);                                          /* Chama a função function, passando sua informação por parâmetro. */
	travelsTree(tree->firstChild, function);                       /* Chama travelsTree() recursivamente passando seu filho como parâmetro. */
	travelsTree(tree->next, function);                             /* Chama travelsTree() recursivamente passando seu irmão como parâmetro. */ 
}


char getInfo(Tree* tree)                                           /* Retorna a informação de um nó tree. */
{
	return tree->info;
}


size_t numberOfLeafs(Tree* tree)                                   /* Retorna o número de folhas que a árvore tree possui. */
{
	if(emptyTree(tree))                                            /* Caso a árvore estiver vazia, retorna 0. */
		return 0;
	
	if(emptyTree(tree->firstChild))                                /* Caso a árvore não possuir filhos, retorna 1 + a chamada recursiva de  */
		return 1 + numberOfLeafs(tree->next);                      /* numberOfLeafs() passando o nó irmão por referência. */
	
	return numberOfLeafs(tree->firstChild) + numberOfLeafs(tree->next); /* Retorna a soma da chamada recursiva de numberOfLeafs() passando */
}                                                                       /* o nó filho por referência com a chamada recursiva de numberOfLeafs() */
                                                                        /* passando o nó irmão por referência. */





