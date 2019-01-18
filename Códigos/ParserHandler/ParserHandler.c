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
#include <strings.h>                                                                  /* Para utilizar strcasecmp() */
#include "ParserHandler.h"

#define BUFFBYTES 32                                                                  /* Quantidade inicial de bytes alocados para buffer. */ 

#define SUCESSO 1
#define ERRO 0


/*
 * getStrings lê e interpreta os caracteres contido no arquivo stream, preenchendo a estrutura preamble e productions.
 */
int getStrings(FILE* stream, Preamble* preamble, Productions* productions)
{
	int i = 0;                                                                        /* Índice do buff. */
	size_t buffSize = BUFFBYTES;                                                      /* Tamanho do buff. */
	char c, *buff;                                                                    /* c é auxiliar. buff é o buffer. */

	buff = (char *) malloc(sizeof(char) * buffSize);                                  /* Alloca espaço de buffSize bytes para buff. */

	if(buff == NULL)
	{
		fprintf(stderr, "Erro ao alocar memoria.\n");                          
		exit(EXIT_FAILURE); 
	}

	while((c = (char) getc(stream)) != EOF)                                           /* Enquanto houver caracteres no arquivo. */
	{
		if(c == '\b' || c == '\r')                                                    /* Caso encontrar carriage return ou backspace, ignora. */
			continue;
			
		if(c == ';')                                                                  /* Caso encontrar ponto e vírgula. */
		{
			while((c = (char) getc(stream)) != EOF && c != '\n');  /* Pula todos os caracteres até encontrar fim de arquivo ou quebra de linha. */
			
			if(c == EOF)                                                              /* Caso chegou ao fim de arquivo, sai do loop. */
				break;
			
			if(i != 0)                                                                /* Se i for diferente de 0, então buff contém algo. */
			{
				buff[i] = '\0';		                                                  /* Coloca caracter '\0' no final de buff. */
				if(!parser(buff, preamble, productions))                              /* Faz o parser de buff. */
				{                                                 
					free(buff);                                  /* Caso algo der errado durante o parser, libera buff e retorna flag de erro. */
					return ERRO;
				}
				i = 0;                                                                /* "Reinicia" o buff, para que possa ser preenchido. */
			}
		}	
		else if(c == '\n')                                                            /* Caso chegou no final de linha. */
		{
			if(i != 0)                                                                /* Caso buff contiver algo a ser escrito. */
			{
				buff[i] = '\0';                                                       /* Coloca o terminador '\0' de string. */
				if(!parser(buff, preamble, productions))         /* Caso algo der errado durante o parser, libera buff e retorna flag de erro. */
				{
					free(buff);
					return ERRO;
				}
				i = 0;                                                                /* "Reinicia" o buff, para que possa ser preenchido. */
			}
		}
		else
		{
			if(i == (int) buffSize - 1)                                    /* Caso buff estiver faltando 1 byte para seu total enchimento. */
			{
				buffSize += BUFFBYTES;                                                /* Aumenta o tamanho de buff. */
				buff = (char *) realloc(buff, buffSize);                              /* Faz um realloc de buff com o novo tamanho de buff. */
				if(buff == NULL)                                                      /* Caso houver erro de realocação. */
				{
					fprintf(stderr, "Erro ao realocar memoria.\n");                   /* Imprime erro. */
					exit(EXIT_FAILURE); 										      /* Aborta o programa retornando EXIT_FAILURE. */
				}
			}
			buff[i++] = c;                                                      /* Coloca o caracter lido do arquivo em buff e incrementa i. */
		}
	}
	
	/* Encontrou fim de arquivo. Saiu do while. Caso buff contém algo e ainda não foi enviado ao parser, i não será 0. */
	
	if(i != 0)                                                                        /* Caso buff contiver algo. */
	{
		buff[i] = '\0';	                                                              /* Coloca '\0' no final de buff. */
		if(!parser(buff, preamble, productions))                                      /* Faz o parser de buff. */
		{
			free(buff);                                            /* Caso algo der errado com o parser, libera buff e retorna flag de erro. */
			return ERRO;
		}
	}
	
	free(buff);                                                                       /* Libera buff. */
	
	return SUCESSO;                                                                   /* Retorna flag de sucesso. */
}


char* myStrdup(const char* s)                           /* http://stackoverflow.com/questions/252782/strdup-what-does-it-do-in-c */
{
	char* d = (char *) malloc(sizeof(char) * (strlen(s) + 1));

	if(d == NULL)
	{
		fprintf(stderr, "Erro ao alocar memoria.\n");                          
		exit(EXIT_FAILURE); 
	}
	
	strcpy(d, s);
	return d;
}


/*
 * Faz o parser de buff e preenche devidamente as estruturas preamble e productions.
 */
int parser(char* buff, Preamble* preamble, Productions* productions)
{
	int i = 0;                                                           /* Índice de buff. */
	
	buff = myStrtok(&buff[i], &i);                           /* buff recebe uma string diferente de espaço e tab para ser analizada a seguir. */
	
	if(buff == NULL)                                /* Caso buff seja NULL, o buff que foi passado como parâmetro só continha espaços ou tabs. */
	{
		return SUCESSO;                                                  /* Retorna sucesso do parser. */
	}
	
		
	if(strcasecmp(buff, ANGLE) == 0)                                     /* Caso buff seja ANGLE. */
	{
		if(!handleAngle(preamble, &buff, &i))                            /* Preenche angle da estrutura preamble. */
			return ERRO;                                                 /* Caso algo de errado ocorra, retorna erro. */
	}
	else if(strcasecmp(buff, ORDER) == 0)                                /* Caso buff seja ORDER. */
	{
		if(!handleOrder(preamble, &buff, &i))                            /* Preenche order da estrutura preamble. */
			return ERRO;                                                 /* Caso algo de errado ocorra, retorna erro. */
	}
	else if(strcasecmp(buff, ROTATE) == 0)                               /* Caso buff seja ROTATE. */
	{
		if(!handleRotate(preamble, &buff, &i))                           /* Preenche rotate da estrutura preamble. */
			return ERRO;                                                 /* Caso algo de errado ocorra, retorna erro. */
	}
	else if(strcasecmp(buff, AXIOM) == 0)                                /* Caso buff seja AXIOM. */
	{
		if(!handleAxiom(productions, &buff, &i))                         /* Preenche axiom da estrutura productions. */
			return ERRO;                                                 /* Caso algo de errado ocorra, retorna erro. */
	}
	else if(strlen(buff) == 1)                                           /* Caso buff seja um único caracter. */
	{	
		if(isSpecialSymbol(buff))                                        /* Caso este caracter seja um símbolo especial. */
		{
			fprintf(stdout, "Invalid use of special symbol '%s'.\n", buff);        /* Imprime erro. */
			return ERRO;                                                           /* Retorna flag de erro. */
		}
		                                                                 /* Caso contrário, preenche productions com uma regra. */
		if(!handleRules(productions, &buff, &i))                         /* Caso algo de errado ocorra, retorna erro. */     
			return ERRO;
	}
	else                                                                 /* Caso contrário, buff contém algo desconhecido. */
	{
		fprintf(stdout, "Unknown keyword '%s'.\n", buff);                /* Imprime erro. */
		return ERRO;                                                     /* Retorna erro. */
	}

	buff = myStrtok(&buff[i], &i);                                       /* Verifica se há mais alguma coisa e buff. */
                                                              
	if(buff != NULL)                                                     /* Se tiver, é porque mais de um comando foi digitado em uma linha. */
	{
		fprintf(stdout, "Unknown keyword or value '%s'.\n", buff);       /* Imprime erro e retorna erro. */ 
		return ERRO;
	}
	
	return SUCESSO;                                                      /* Retorna sucesso. */
}


/* 
 * Semelhante a strtok(). 
 * Exemplo: buff contém " hello world"
 * Chamando myStrtok() uma vez, retornará um ponteiro para char apontando para o 'h' e terá preenchido o espaço após 'hello' com '\0'.
 * O i não deve ser modificado entre as chamadas, pois armazena a posição para um novo myStrtok, neste caso, buff[*i] == 'w'.
 * Na próxima chamada a myStrtok(), retornará um ponteiro para char apontando para 'w'. Como foi encontrado o '\0' na string, i é setado com uma
 * flag, indicando que a string chegou ao fim.
 * Para chamar myStrtok, deve deve utilizar os parâmetros e o retorno da seguinte forma: buff = myStrtok(&buff[i], &i);
 */
char* myStrtok(char* buff, int* i)
{
	if(*i == -1)                                                            /* Verifica se a string já foi lida até o fim. */
		return NULL;                                                        /* Se sim, retorna NULL. */
		
	*i = 0; 
	while(*buff != '\0' && (*buff == ' ' || *buff == '\t')) buff++;         /* Pula os espaços e tabs, apontanto para o próximo caracter. */    
	
	if(*buff == '\0')                                                       /* Se buff só continha espaços. */
	{
		*i = -1;                                                            /* Marca i com a flag, indicando final da string. */
		return NULL;                                                        /* Retorna NULL. */
	}
	
	while(buff[*i] != '\0' && buff[*i] != ' ' && buff[*i] != '\t') (*i)++;  /* Pula os caracteres válidos, porém, andando com o índice i. */

	if(buff[*i] == '\0')                                                    /* Se chegou ao fim de buff. */
	{
		*i = -1;                                                            /* Marca i com a flag, indicando final da string. */
		return buff;                                                        /* Retorna buff, apontanto para o início de uma string válida. */
	}

	buff[*i] = '\0';                                                        /* Preenche o espaço ou tab com '\0'. */
	(*i)++;                                                                 /* Incrementa i. */

	return buff;                                                            /* Retorna buff, apontanto para o início de uma string válida. */
}


int handleAngle(Preamble* preamble, char** buff, int* i)
{
	long int intConverted;                                                  /* Armazenará valor inteiro convertido. */
	
	*buff = myStrtok(*buff + *i, i);                                        /* *buff + *i == &(*buff)[*i] */ 

	if(*buff == NULL)                                                       /* Caso não houver nada após 'angle'. */
	{
		fprintf(stdout, "Missing value for angle.\n");                      /* Imprime erro. */
		return ERRO;                                                        /* Retorna flag de erro. */
	}
	
	if(intConvertion(&intConverted, *buff))                                 /* Faz a converção do valor encontrado para inteiro. */
	{
		if(3 <= intConverted && intConverted <= 360)                        /* Checa se angle está dentro dos limites. */
		{
			preamble->angle = (int) intConverted;                     /* Preenche valor de angle da estrutura preamble com o valor convertido. */
		}
		else
		{ 
			fprintf(stdout, "Angle must be between 3 and 360.\n");          /* Caso estiver fora dos limites, imprime erro e retorna erro. */  
			return ERRO;
		}
	}	
	else
	{
		fprintf(stdout, "Angle value '%s' not a number or integer.\n", *buff);    /* Caso valor de angle não for um número ou número inteiro, */
		return ERRO;                                                              /* imprime erro e retorna erro. */
	}
	
	return SUCESSO;                                                         /* Retorna flag de sucesso. */
}


int handleOrder(Preamble* preamble, char** buff, int* i)
{
	long int longConverted;                                                 /* Armazenará valor inteiro convertido. */
	
	*buff = myStrtok(*buff + *i, i);

	if(*buff == NULL)                                                       /* Caso não houver nada após 'order'. */
	{
		fprintf(stdout, "Missing value for order.\n");                      /* Imprime erro. */
		return ERRO;                                                        /* Retorna flag de erro. */
	}
	
	if(intConvertion(&longConverted, *buff))                                /* Faz a converção do valor encontrado para inteiro. */
	{
		if(0 <= longConverted && longConverted <= 2147483647)               /* Checa se order está dentro dos limites. */
		{
			preamble->order = longConverted;                        /* Preenche valor de order da estrutura preamble com o valor convertido. */
		}
		else
		{ 
			fprintf(stdout, "Order must be between 0 and 2147483647.\n");   /* Caso estiver fora dos limites, imprime erro e retorna erro. */ 
			return ERRO;
		}
	}	
	else
	{
		fprintf(stdout, "Order value '%s' not a number or integer.\n", *buff);    /* Caso valor de order não for um número ou número inteiro, */
		return ERRO;                                                              /* imprime erro e retorna erro. */
	}
	
	return SUCESSO;                                                         /* Retorna flag de sucesso. */
}


int handleRotate(Preamble* preamble, char** buff, int* i)
{
	double floatConverted;                                                  /* Armazenará valor ponto flutuante convertido. */

	*buff = myStrtok(*buff + *i, i);

	if(*buff == NULL)                                                       /* Caso não houver nada após 'rotate'. */
	{
		fprintf(stdout, "Missing value for rotate.\n");                     /* Imprime erro. */
		return ERRO;                                                        /* Retorna flag de erro. */
	}
	
	if(floatConvertion(&floatConverted, *buff))                             /* Faz a converção do valor encontrado para double. */
	{
		if(-360 < floatConverted && floatConverted < 360)                   /* Checa se rotate está dentro dos limites. */
		{
			preamble->rotate = (float) floatConverted;               /* Preenche valor de rotate da estrutura preamble com o valor convertido. */
		}
		else
		{ 
			fprintf(stdout, "Rotate value must be inside -360...360.\n");   /* Caso estiver fora dos limites, imprime erro e retorna erro. */ 
			return ERRO; 
		}
	}	
	else
	{
		fprintf(stdout, "Rotate value '%s' not a number.\n", *buff);   /* Caso valor de rotate não for um número, imprime erro e retorna erro. */
		return ERRO;
	}

	return SUCESSO;                                                         /* Retorna flag de sucesso. */   
}


int handleAxiom(Productions* productions, char** buff, int* i)  
{
	*buff = myStrtok(*buff + *i, i);

	if(*buff == NULL)                                                       /* Caso não houver nada após 'axiom'. */
	{
		fprintf(stdout, "Missing value for axiom.\n");                      /* Imprime erro. */
		return ERRO;                                                        /* Retorna flag de erro. */
	}
	
	if(isKeyword(*buff) || strchr(*buff, (int)'=') != NULL)                 /* Caso buff for palavra chave ou '=' */
	{
		fprintf(stdout, "Axiom cannot be a reserved word or special symbol '%s'.\n", *buff);    /* Imprime erro. */
		return ERRO;                                                                            /* Retorna flag de erro. */
	}
	
	productions->axiom = myStrdup(*buff);                                   /* Preenche axiom da estrutura productions com o valor encontrado. */

	return SUCESSO;                                                         /* Retorna flag de sucesso. */                             
}


int handleRules(Productions* productions, char** buff, int* i)
{
	Rule* rule;
	char p = **buff;              /* Como visto antes (condição para chamar handleRules()), *buff tem comprimento 1, então p armazena **buff. */
	
	*buff = myStrtok(*buff + *i, i);                                        /* Captura próxima string de buff. */

	if(*buff == NULL || strcmp(*buff, "=") != 0)                            /* Caso buff seja vazio, ou não seja o símbolo de '='. */
	{
		fprintf(stdout, "Expected '=' after '%c'.\n", p);                   /* Imprime erro. */
		return ERRO;                                                        /* Retorna flag de erro. */
	}
	
	rule = createRule();                                                    /* Cria uma estrutura Rule. */
	rule->p = p;                                                            /* Copia p para a regra rule. */
	
	*buff = myStrtok(*buff + *i, i);                                        /* Captura a próxima string de buff. */
	
	if(*buff != NULL)                                                       /* Se buff for diferente de NULL, ou seja, conter algo. */
	{
		if(isKeyword(*buff) || strcmp(*buff, "=") == 0)                     /* Caso buff for palavra chave ou símbolo '='. */
		{
			fprintf(stdout, "Rule cannot be a reserved word or symbol'%s'.\n", *buff);       /* Imprime erro. */
			return ERRO;                                                                     /* Retorna flag de erro. */
		}
		rule->s = myStrdup(*buff);                                          /* String da regra rule recebe uma cópia de buff. */
	}
	
	/* Caso buff for NULL, então p = 'vazio'. Como ao criar rule em createRule() rule->s está apontanto para NULL, */
	/* então esta regra p, tem como substituto 'nada'.                                                             */
	
	insertBegList(&productions->rules, rule);                      /* Insere a nova regra criada na lista de regras da estrutura productions. */

	return SUCESSO;                                                         /* Retorna flag de sucesso. */
}


int isKeyword(const char* str)                                              /* Verifica se str é igual a alguma palavra reservada. */
{
	return (!strcasecmp(str, ANGLE) || !strcasecmp(str, ORDER) || !strcasecmp(str, ROTATE) || !strcasecmp(str, AXIOM));
}


int isSpecialSymbol(const char* str)                                        /* Verifica se str contém algum símbolo especial. */
{
	return strpbrk(str, SPECIALSYMBOLS) != NULL;
}


int intConvertion(long int* converted, const char* str)                     /* Faz a conversão de uma string em long int. */
{
	char* endptr;

	*converted = strtol(str, &endptr, 10);
	
	return *endptr == '\0';                                                 /* Retorna 1 em caso de sucesso da conversão e 0 caso contrário. */
}


int floatConvertion(double* converted, const char* str)                     /* Faz a conversão de uma string em double. */
{
	char* endptr;
	
	*converted = strtod(str, &endptr);
	
	return *endptr == '\0';                                                 /* Retorna 1 em caso de sucesso da conversão e 0 caso contrário. */
}


Rule* createRule(void)                                                      /* Cria uma estrutura Rule e retorna seu endereço. */
{
	Rule* new = (Rule *) malloc(sizeof(Rule));
	
	if(new == NULL)
	{	
		fprintf(stderr, "\nError while allocating memory.\n\n");
		exit(EXIT_FAILURE);
	} 
	new->s = NULL;                                        /* A função free() não tem efeito sobre um ponteiro para NULL. (Fonte: man malloc) */

	return new;   
}


void freeRule(void* rule)                                 /* Libera rule. Rule é passada como void* pois é utilizada como callback. */
{
	Rule* r = (Rule *) rule; 
	
	free(r->s);
	free(r);
}


void printElementRule(void* rule)                         /* Imprime rule. Função utilizada para debugar durante o desenvolvimento do código. */
{
	Rule* r = (Rule *) rule;
	printf("[%c]=[%s]\n", r->p, r->s);
}





