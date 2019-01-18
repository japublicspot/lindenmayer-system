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
#include "GenericList.h"


int emptyList(List* l)                                        /* Verifica se a lista l está vazia. */
{
	return l == NULL;
}


List* createEmptyList(void)                                   /* Cria uma lista vazia. */
{
	return NULL;
}


List* createNode(void* info)                                  /* Cria um nó do tipo List com a informação passada como parâmetro. */
{
	List* new = (List *) malloc(sizeof(List));                              
	
	if(new == NULL)
	{	
		fprintf(stderr, "\nError while allocating memory.\n\n");
		exit(EXIT_FAILURE);
	}
	
	new->info = info;  
	new->next = NULL;

	return new;
}


void insertBegList(List** l, void* info)                     /* Insere um nó com a informação info no início da lista. */
{
	List* new = createNode(info);                            /* Cria um nó com a informação info. */
	
	new->next = *l;                                          /* O nó recentemente, tem como próximo o primeiro elemento da lista *l, */
	*l = new;                                                /* Se a lista *l for vazia (NULL), então aponta para NULL. */
}                                                            /* Atualiza o ponteiro *l para o primeiro elemento da lista. */


void* removeBegList(List** l)                                /* Remove um nó do início da lista. */
{
	List* temp;                                  
	void* info;
	
	if(*l == NULL)                                           /* Caso a lista *l estiver vazia. */
		return NULL;						                 /* Retorna NULL. */
		
	info = (*l)->info;	                                     /* info armazana a informação contida no primeiro nó da lista. */
	temp = (*l)->next;                                       /* temp armazena temporariamente o endereço do próximo nó. */
	free(*l);                                                /* Libera a memória que o primeiro nó consumia. */
	*l = temp;                                               /* Atualiza o ponteiro da lista *l para o atual primeiro elemento da lista. */

	return info;                                             /* Retorna a informação que estava armazenada no primeiro nó. */
}


void freeList(List** l, void (* freeInfo) (void *))    /* Percorre a lista *l liberando todos os seus nós. */
{
	List* temp;

	while(*l != NULL)                          /* Enquanto a lista *l não estiver vazia. */
	{
		temp = (*l)->next;                     /* Armazena temporariamente o ponteiro para o próximo nó. */
		freeInfo((*l)->info);                  /* Chama a função passada como parâmetro para liberar a informação que o usuário do TAD criou. */
		free(*l);                              /* Libera o nó corrente. */
		*l = temp;                             /* *l é atualizado, apontando para o próximo nó. */
	}										   /* Quando a lista estiver vazia, *l apontará para vazio (NULL). */
}


void travelsList(List* l, void (* function) (void *))   /* Percorre uma lista l e aplica a função de callback function para cada informação. */
{	
	for(; l != NULL; l = l->next)                       /* Percorre a lista até seu fim. */
	{
		function(l->info);                              /* Chama o callback, passando a referência para a informação. */
	}
}


/* http://stackoverflow.com/questions/400951/does-c-have-a-foreach-loop-construct */
/*
 * A solução encontrada para um foreach em c no stackoverflow, foi definir esta MACRO: 
 *
 * #define forEachListItem(nodeInfo, list) \
 *		for((nodeInfo) = (list)->info; (list) != NULL; (list) = (list)->next, (nodeInfo) = (list)->info)
 *
 * Mas não me serviu muito bem, então preferi fazer assim: 
 */

const void* nextInfo(List** l)                     /* Espécie de foreach. Apenas olha a próxima informação contida na lista. */
{                                       
	const void* info;	
	
	if(*l == NULL)                                 /* Se a lista estiver vazia. */
		return NULL;                               /* Retorna NULL. */
	
	info = (*l)->info;                             /* info armazena a referência para a informação do nó. */
	*l = (*l)->next;                               /* Atualiza o ponteiro *l para o próximo nó. */
	
	return info;                                   /* Retorna a referência para a informação. Note que é const, ou seja, não se pode modificar */
}                                                  /* a memória apontada por info. */
		
	
	
	
