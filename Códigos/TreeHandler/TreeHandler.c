/*
 * Aluno: Vinicius Ferraço Arruda
 * Curso: Ciência da Computação
 * E-mail: viniciusferracoarruda@gmail.com
 * Disciplina: Estrutura de Dados I
 * Professor: Thomas W. Rauber
 * Data: 28 de junho de 2015
 */

#include <stdio.h>
#include <string.h>
#include "TreeHandler.h"
#include "../ParserHandler/ParserHandler.h"
#include "../GenericList/GenericList.h"


void insertStr(Tree* tree, char* str)                /* Insere a string str na lista de filhos da árvore tree. */
{
	if(emptyTree(tree))                              /* Caso a árvore estiver vazia, retorna. */
		return;
		
	if(str == NULL)                                  /* Caso str apontar para NULL. */
	{
		insertEnd(tree, ' ');                        /* Insere espaço (representando nada) no final da lista de filhos da árvore. */
	}
	else                                             /* Caso contrário. */
	{
		for( ; *str != '\0'; str++)                  /* Para cada caracter contido em str, */
			insertEnd(tree, *str);                   /* insere no final da lista de filhos da árvore. */
	}
}


void handlerLeaf(Tree* tree, void* ruleList)         /* Função utilizada como callback. */
{
	const Rule* rule;                               
	List* rList = (List *) ruleList;               
	
	while((rule = nextInfo(&rList)) != NULL)         /* Para cada regra na lista de regras, */
	{
		if(getInfo(tree) == rule->p)                 /* verifica se a informação daquele nó da árvore é igual a regra. */
		{
			insertStr(tree, rule->s);                /* Se sim, insere na árvore a string da regra. */
			return;                                  /* Retorna, pois só deve existir apenas um p. */
		}                                            /* Caso o usuário digitar p repetido (p = REGRA), o programa vai considerar o último. */
	}                                                /* Este comportamento de considerar o último, é semelhante ao do applet do cgjennings. */
}


void printTree(char info)                            /* Imprime a informação da árvore. */
{
	printf("%c", info);
}


void getFinalString(Tree* tree, void* finalStr)      /* Função utilizada como callback. Acumula as informações dos nós em finalStr. */
{
	char** str = finalStr;
	
	**str = getInfo(tree);                           /* Recebe a informação do nó. */
	(*str)++;                                        /* Incrementa o ponteiro. */
	**str = '\0';                                    /* Coloca '\0' para indicar fim de string. */
}



