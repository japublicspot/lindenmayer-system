/*
 * Aluno: Vinicius Ferraço Arruda
 * Curso: Ciência da Computação
 * E-mail: viniciusferracoarruda@gmail.com
 * Disciplina: Estrutura de Dados I
 * Professor: Thomas W. Rauber
 * Data: 28 de junho de 2015
 */

#ifndef PARSERHANDLER_H                              /* Caso não tenha definido os headers contido em ParserHandler.h. */
#define PARSERHANDLER_H                              /* Define os headers contido em ParserHandler.h. */

#include "../GenericList/GenericList.h"

#define ANGLE "angle"  
#define ORDER "order"
#define AXIOM "axiom"
#define ROTATE "rotate"
#define SPECIALSYMBOLS  "=+-!|[]<>@/\\_c" 			      

typedef struct                                       /* Estrutura do preamble. */
{
	int angle;                                       /* Angulo. */
	long int order;                                  /* Order. */
	double rotate;                                   /* Rotate. */
} Preamble;


typedef struct                                       /* Estrutura do productions. */
{
	char* axiom;                                     /* Axioma. */
	List* rules;                                     /* Lista de regras. */
} Productions;


typedef struct                                       /* Estrutura da regra. */
{
	char p; /* p = s */                              /* p. */
	char* s;                                         /* String da regra p. */
} Rule;


int getStrings(FILE* stream, Preamble* preamble, Productions* productions);
int parser(char* buff, Preamble* preamble, Productions* productions);

int handleAngle(Preamble* preamble, char** buff, int* i);
int handleOrder(Preamble* preamble, char** buff, int* i);
int handleRotate(Preamble* preamble, char** buff, int* i);
int handleAxiom(Productions* productions, char** buff, int* i);
int handleRules(Productions* productions, char** buff, int* i);

int isKeyword(const char* str);
int isSpecialSymbol(const char* str);

char* myStrdup(const char* s);
char* myStrtok(char* buff, int* i);
int intConvertion(long int* converted, const char* str);
int floatConvertion(double* converted, const char* str);

void freeRule(void* rule);
Rule* createRule(void);
void printElementRule(void* rule);

#endif

