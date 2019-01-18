/*
 * Aluno: Vinicius Ferraço Arruda
 * Curso: Ciência da Computação
 * E-mail: viniciusferracoarruda@gmail.com
 * Disciplina: Estrutura de Dados I
 * Professor: Thomas W. Rauber
 * Data: 28 de junho de 2015
 */
 
 
#ifndef VARIABLETREE_H                          /* Caso não tenha definido os headers contido em VariableTree.h. */
#define VARIABLETREE_H                          /* Define os headers contido em VariableTree.h. */

struct tree                                     /* Estrutura da árvore Tree. */
{
	char info;                                  /* Informação: Apenas um único caracter. */
	struct tree* firstChild;                    /* Ponteiro para o primeiro filho. */
	struct tree* lastChild;                     /* Ponteiro para o último filho. */
	struct tree* next;                          /* Ponteiro para o irmão. */
};

/* Obs: Foi preferido criar um ponteiro para o último filho para facilitar na montagem da árvore. */

typedef struct tree Tree;


int emptyTree(Tree* tree);
Tree* createTree(char info);
void insertEnd(Tree* tree, char info);
void freeTree(Tree** tree);
void travelsTreeLeaf(Tree* tree, void (* function)(Tree *, void *), void* arg);
void travelsTree(Tree* tree, void (* function)(char));
char getInfo(Tree* tree);
size_t numberOfLeafs(Tree* tree);

#endif

