/*
 * Aluno: Vinicius Ferraço Arruda
 * Curso: Ciência da Computação
 * E-mail: viniciusferracoarruda@gmail.com
 * Disciplina: Estrutura de Dados I
 * Professor: Thomas W. Rauber
 * Data: 28 de junho de 2015
 */
 
#ifndef LIST_H                   /* Caso não tenha definido os headers contido em GenericList.h. */
#define LIST_H                   /* Define os headers contido em GenericList.h. */


struct list                      /* Estrutura de um nó do tipo List. */
{
	void* info;                  /* Ponteiro genério para a informação. */
	struct list* next;           /* Ponteiro para o próximo nó da lista. */
};

typedef struct list List;


int emptyList(List* l);
List* createEmptyList(void);
void insertBegList(List** l, void* info);
void freeList(List** l, void (* freeInfo) (void *));
void travelsList(List* l, void (* function) (void *));
List* createNode(void* info);
void* removeBegList(List** l);
const void* nextInfo(List** l);

#endif 


