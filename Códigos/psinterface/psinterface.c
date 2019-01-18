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
#include <string.h>
#include "psinterface.h"




static int isValidCommand(char c)                      /* Verifica se um caracter c pertence à uma string que representa os comandos válidos. */
{	
	return strchr(TURTLE_GRAPHICS_SYMBOLS,(int)c) != NULL;
}


static void finalStringPrint(FILE *ps, const char *finalStr) /* Imprime a string final no arquivo. */
{
	int count = 0;                                           /* Inicializa o contador para imprimir uma quantia de até 30 caracteres por linha. */
	
	for(; *finalStr != '\0'; finalStr++)                     /* Percorre a string, até o final dela. */
	{
		if(isValidCommand(*finalStr))                        /* Verifica se o caracter é válido. */
		{
			if(*finalStr == '[')                             /* Se for um push ([), converte para S (STORE). */
				fprintf(ps, "S ");
			else if(*finalStr == ']')                        /* Se for um pop (]), converte para R (RESTORE). */
				fprintf(ps, "R ");
			else                                             /* Caso contrário. */
				fprintf(ps, "%c ", *finalStr);               /* Imprime este caracter no arquivo. */
				
			if(count == 29)                                  /* Caso count igual a 29 (Lembrando que passou por 0, então foi um total de 30). */
			{
				fprintf(ps, "\n");                           /* Quebra linha, imprimindo 30 caracteres por linha. */
				count = 0;                                   /* Zera o contador. */
			}
			else                                             /* Caso contrário. */
				count++;                                     /* Incrementa o contador. */
		}
	}
	fprintf(ps,"\n");                                        /* Quebra linha. */
}


static void preambulo(FILE* ps, double angle, long int order, double rotimage)   /* Imprime o preâmbulo do arquivo PS. */
{	
	fprintf(ps,"%%!PS-Adobe-2.0 EPSF-2.0\n");
	fprintf(ps,"%%%%BoundingBox: 0 0 800 1000\n");
	fprintf(ps,"\n\n\n");
	fprintf(ps,"%% Preambulo\n\n");
	fprintf(ps,"\n");
	fprintf(ps,"/angle %.2f def\n", angle);
	fprintf(ps,"/order %ld def\n", order);
	fprintf(ps,"/rotateimage %.2f def\n", rotimage);
	fprintf(ps,"\n");
	fprintf(ps,"/a { 360 angle div } def\n");

	fprintf(ps,"/scal { 2 order neg exp } def\t%% 1/2^order\n");
	fprintf(ps,"/sinv { 2 order exp } def\t%% 2^order\n");
	fprintf(ps,"\n");
	fprintf(ps,"/len 72 def\n");
	fprintf(ps,"\n");
	fprintf(ps,"\n");
	fprintf(ps,"/F { len 0 rlineto } def\n");
	fprintf(ps,"/G   { len 0 rmoveto } def\n");
	fprintf(ps,"/+ {a rotate} def\n");
	fprintf(ps,"/- {a neg rotate} def\n");
	fprintf(ps,"\n");
	fprintf(ps,"/S { gsave } def\n/R { stroke grestore } def\n");
	fprintf(ps,"\n");
	fprintf(ps,"/n {newpath} def\n");
	fprintf(ps,"/l {lineto} def\n");
	fprintf(ps,"/m {moveto} def\n");
	fprintf(ps,"/s {stroke} def\n");
	fprintf(ps,"\n");
	fprintf(ps,"/pageLetterCenter { 297.72 421.2 } def\n");
	fprintf(ps,"/linewidth 0.1 def\n");
	fprintf(ps,"\n");
	fprintf(ps,"gsave\n");
	fprintf(ps,"\n");
	fprintf(ps,"pageLetterCenter translate\n");
	fprintf(ps,"rotateimage rotate\n");
	fprintf(ps,"scal scal scale\n");
	fprintf(ps,"linewidth setlinewidth\n");
	fprintf(ps,"newpath 0 0 moveto\n");
}


void genps(double angle, long int order, double rotimage, const char *finalstr, const char *psfname)  /* Gera o arquivo PS. */
{	
	FILE* ps = fopen(psfname, "w");
	
	if(ps == NULL)
	{
		fprintf(stderr, "Error while opening file '%s'.\n", psfname);
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Gerando arquivo de saida PostScript '%s' ...", psfname);

	preambulo(ps, angle, order, rotimage);
	
	finalStringPrint(ps, finalstr);
	
	fprintf(ps, "stroke\n\ngrestore\n\nshowpage\n");
	fprintf(ps, "quit\n");
	
	fclose(ps);
	
	fprintf(stdout, "Done!\n\n");
}



static void finalStringPrintTurtle(FILE *ps, const char *turtlestr)         /* Imprime a string final no arquivo. */
{	
	for(; *turtlestr != '\0'; turtlestr++)	                                /* Passa por toda a string final. */
		fprintf(ps, "%c", *turtlestr);                                      /* Imprime caracter a caracter no arquivo ps. */
}


void genpsTurtle(double angle, long int order, double imagerotate, const char *turtlestr, const char *psfname)   /* Gera o arquivo Turtle. */
{
	FILE* ps = fopen(psfname, "w");
	
	if(ps == NULL)
	{
		fprintf(stderr, "Error while opening file '%s'.\n", psfname);
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Gerando arquivo de saida PostScript '%s' ...", psfname);

	preambulo(ps, angle, order, imagerotate);
	
	finalStringPrintTurtle(ps, turtlestr);
	
	fprintf(ps, "stroke\n\ngrestore\n\nshowpage\n");
	fprintf(ps, "quit\n");
	
	fclose(ps);
	
	fprintf(stdout, "Done!\n\n");
}



