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
#include "Stack.h"
#include "../GenericList/GenericList.h"


/* Cria uma estrutura Turtle e retorna seu endereço. */
Turtle* createTurtle(double x, double y, double orientation, double length, int color, int counterclockwise)
{
	Turtle* new = (Turtle *) malloc(sizeof(Turtle)); 

	if(new == NULL)
	{
		fprintf(stderr, "Error while allocating memory.\n");
		exit(EXIT_FAILURE);
	}
	
	new->x = x;                                                 /* Inicializa new com os valores passados como parâmetro. */
	new->y = y;
	new->orientation = orientation;
	new->length = length;
	new->color = color;
	new->counterclockwise = counterclockwise;
	
	return new;
}


Stack* createEmptyStack(void)
{
	return createEmptyList();
}


void push(Stack** stack, Turtle* turtle)
{
	insertBegList(stack, (void *) turtle);	
}


Turtle* pop(Stack** stack)
{
	return (Turtle *)removeBegList(stack);	 
}


int emptyStack(Stack* stack)
{
	return emptyList(stack);
}


void freeStack(Stack** stack)
{
	freeList(stack, free);
}


