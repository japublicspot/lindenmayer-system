/*
 * Aluno: Vinicius Ferraço Arruda
 * Curso: Ciência da Computação
 * E-mail: viniciusferracoarruda@gmail.com
 * Disciplina: Estrutura de Dados I
 * Professor: Thomas W. Rauber
 * Data: 28 de junho de 2015
 */

#ifndef STACK_H                                     /* Caso não tenha definido os headers contido em Stack.h. */
#define STACK_H                                     /* Define os headers contido em Stack.h. */
#include "../GenericList/GenericList.h"


struct turtle                                       /* Estrutura Turtle. */
{
	double x;                                       /* Posição x. */
	double y;                                       /* Posição y. */
	double orientation;                             /* Orientação (Ângulo). */
	double length;                                  /* Comprimento da linha. */
	int color;                                      /* Cor da linha. */
	int counterclockwise;                           /* Flag para sentido horário ou anti-horário. */
};

typedef struct turtle Turtle;

typedef List Stack;

Turtle* createTurtle(double x, double y, double orientation, double length, int color, int counterclockwise);
Stack* createEmptyStack(void);
void push(Stack** stack, Turtle* turtle);
Turtle* pop(Stack** stack);
int emptyStack(Stack* stack);
void freeStack(Stack** stack);

#endif 

