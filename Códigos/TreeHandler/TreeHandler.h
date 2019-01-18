/*
 * Aluno: Vinicius Ferraço Arruda
 * Curso: Ciência da Computação
 * E-mail: viniciusferracoarruda@gmail.com
 * Disciplina: Estrutura de Dados I
 * Professor: Thomas W. Rauber
 * Data: 28 de junho de 2015
 */
 
#ifndef TREEHANDLER_H                            /* Caso não tenha definido os headers contido em TreeHandler.h. */
#define TREEHANDLER_H                            /* Define os headers contido em TreeHandler.h. */

#include "../VariableTree/VariableTree.h"

void insertStr(Tree* tree, char* str);  
void printTree(char info);
void handlerLeaf(Tree* tree, void* ruleList);
void getFinalString(Tree* tree, void* finalStr);

#endif

