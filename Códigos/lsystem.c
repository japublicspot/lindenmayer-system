/*
 * Aluno: Vinicius Ferraço Arruda
 * Curso: Ciência da Computação
 * E-mail: viniciusferracoarruda@gmail.com
 * Disciplina: Estrutura de Dados I
 * Professor: Thomas W. Rauber
 * Data: 28 de junho de 2015
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ParserHandler/ParserHandler.h"  
#include "VariableTree/VariableTree.h" 
#include "TreeHandler/TreeHandler.h" 
#include "GenericList/GenericList.h" 
#include "Stack/Stack.h" 
#include "psinterface/psinterface.h"


#define BYTES 65536                                     /* Quantidade inicial de bytes a serem alocados para a string final na segunda parte. */ 
#define NEWBYTES 32768                          /* Quantidade de bytes a serem adicionados à quantidade inicial caso necessite de um realloc. */
#define UNIT 72.0                                       /* Unidade básica do comprimento de uma linha. */
#define toRadians(degrees) ((degrees) * M_PI / 180.0)   /* Converte um ângulo em graus para radianos. */
#define toDegrees(angle) (360.0/(angle))                /* Converte o ângulo inserido no preâmbulo para a medida em graus a ser utilizada. */
#define x1(x0, alpha, z) ((x0) + (z)*cos(alpha))        /* Calcula próxima posição x. */
#define y1(y0, alpha, z) ((y0) + (z)*sin(alpha))        /* Calcula próxima posição y. */

                                                        /* http://mcgraweng.com/Postscript%20Colors.pdf (No cabeçalho). */
#define colorNumber(n) (n % NUMCOLORS)                  /* Dado um valor n, retorna o número da cor correspondente (0 até NUMCOLORS - 1). */
#define NUMCOLORS 256                                                        
#define rgbToPS(n) (n / 255.0)                          /* Converte um valor em rgb (0 até 255) para rgb do postscript (0 até 1). */


#define charToSecondPart "FG+-[]><c@|!IQ.0123456789"    /* Caracteres válidos para a segunda parte. */


char* firstPart(FILE* input, Preamble* preamble);
char* secondPart(char* str, double angle);

int colorIncrement(int n, int value);
int colorDecrement(int n, int value);

void removeExtraCaracters(char* str);



int main(int argc, char** argv)                         /* main. */
{
	FILE* input;                                        /* Arquivo de entrada com preâmbulo e produções. */
	Preamble preamble;                                  /* Estrutura preâmbulo. */
	char* firstStr;                                     /* Armazenará a referência para a primeira string. Formato PS. */
	char* secondStr;                                    /* Armazenará a referência para a segunda string. Formato Turtle. */
	
	if(argc < 4)                                        /* Espera 4 entradas de linha de comando. */
	{
		printf("Usage: %s [Input File Name] [Output PS File Name] [Output Turtle File Name]\n", argv[0]);
		return 0; 
	}

	input = fopen(argv[1], "r");                        /* Abre arquivo de entrada para leitura. */

	if(input == NULL)
	{
		fprintf(stderr, "Erro ao abrir arquivo %s.\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	
	
	firstStr = firstPart(input, &preamble);             /* Chama firstPart() e recebe string final gerada pela primeira parte. */
	
	fclose(input);                                      /* Fecha arquivo de entrada. */
	
	if(firstStr != NULL)                                /* Caso firstStr não seja NULL. */
	{
		genps(preamble.angle, preamble.order, preamble.rotate, firstStr, argv[2]);  /* Gera arquivo PS com a string final da primeira parte. */
	
		secondStr = secondPart(firstStr, preamble.angle);             /* Chama secondPart() e recebe string final gerada pela segunda parte. */
		
		if(secondStr != NULL)                                                       /* Caso secondStr não seja NULL. */
		{
			                                                                        /* Gera arquivo PS com a string final da segunda parte. */
			genpsTurtle(preamble.angle, preamble.order, preamble.rotate, secondStr, argv[3]);
		
			free(secondStr);                                                        /* Libera memória ocupada por secondStr. */
		}
		/* Caso secondStr seja NULL, algum erro de leitura ocorreu, abortando o programa com sucesso, pois o erro foi do usuário, e não do */     
		/* sistema.                                                                                                                        */
		
		free(firstStr);                                                             /* Libera memória ocupada por firstStr. */
	}
	/* Caso firstStr seja NULL, ou axioma é vazio, o que não irá gerar nada, ou algum erro de leitura ocorreu, abortando o programa com */
	/* sucesso, pois o erro foi do usuário, e não do sistema. */

	return 0;                                                                       /* Encerra o programa com sucesso. */
}


/*
 * Executa a primeira parte do trabalho, gerando uma string final que será passada para a função secondPart() posteriormente.
 * Esta string final será passada por um filtro antes de ser retornada, eliminando possiveis regras. Ex: ++FFX (X será eliminado).
 * Quando retornada, ela será escrita no arquivo postscript de saída, porém será aplicada à ela um novo filtro, eliminando qualquer caracter 
 * que não seja "FG+-[]", pois a primeira parte só implementa esses comandos.
 * WARNING: Se a string final, ao ser escrita no arquivo de saída, conter valores diferente de "FG+-[]", eles serão excluidos, e consequentemente
 * a imagem gerada será diferente da desejada. Foi preferido implementar desta maneira, pois sempre que houvesse algum caracter diferente de 
 * "FG+-[]" e o programa abortasse relatando erro, a segunda parte do trabalho nunca seria executada com os comandos extras (Gerando a imagem 
 * desejada).
 * Retorna um ponteiro para a string final com os passos de tartaruga em caso de sucesso.
 * Retorna NULL em caso de algum erro. 
 */
char* firstPart(FILE* input, Preamble* preamble)
{
	Productions* productions;    /* Ponteiro para estrutura productions. */
	Tree* tree;                  /* Ponteiro para árvore. */
	char* finalStr;              /* Ponteiro para a string final que será gerada. */
	char* saveptr;               /* Ponteiro auxiliar, para salvar a referência entre chamadas de uma função. */
	int i;                       /* Variável de índice de loop. */

	productions = (Productions *) malloc(sizeof(Productions));    /* Aloca um espaço para uma estrutura productions. */

	if(productions == NULL)	
	{
		fprintf(stderr, "Error while allocating memory.\n");
		exit(EXIT_FAILURE);
	}
    
    /* Inicializa productions. No caso do arquivo de entrada não fornecer angle, order, rotate ou axiom, eles permanecerão como a seguir. */
	preamble->angle = 3;                                         /* Valor mínimo para angle.  */ 
	preamble->order = 0;                                         /* Valor mínimo para order.  */
	preamble->rotate = 0.0;	                                     /* Valor inicial para rotate. */

	productions->axiom = NULL;                                   /* Axioma vazio.                      */
	productions->rules = createEmptyList();                      /* rules aponta para uma lista vazia. */


	if(!getStrings(input, preamble, productions))          /* Chama a função para ler o arquivo de entrada e preencher preamble e productions. */
	{
		fprintf(stdout, "Something went wrong while parsing. Please, check your syntax.\n");   /* Caso houver algum erro.        */
		free(productions->axiom);                                                              /* Imprime mensagem de erro.      */
		freeList(&productions->rules, freeRule);                                               /* Libera toda a memória ocupada. */
		free(productions);	                                                                   /* Retorna NULL.                  */
		return NULL;
	}

#if 0	                                             /* Para debugar. Imprime as informações contidas em preamble e productions. */
	printf("\n");
	printf("\norder:[%ld]", preamble->order);
	printf("\nrotate:[%f]", preamble->rotate);
	printf("\nangle:[%d]", preamble->angle); 
	printf("\naxiom:[%s]", productions->axiom);
	printf("\nrules:\n{");
	travelsList(productions->rules, printElementRule);
	printf("}\n\n");
#endif	
	
	
	
	tree = createTree('\0');                                      /* Cria raiz da árvore. */

	insertStr(tree, productions->axiom);                          /* Insere o axioma na árvore. */
	
	
	for(i = 0; i < preamble->order; i++)                          /* Aplica as regras em um número de vezes especificado em order. */
	{
		travelsTreeLeaf(tree, handlerLeaf, productions->rules);   /* Passa por todas as folhas da árvore, e para cada uma, */
	}                                                             /* chama a função de callback handlerLeaf. */
	
	finalStr = (char *) malloc(sizeof(char) * (numberOfLeafs(tree) + 1));  /* Aloca um espaço de memória para comportar a string final. */
	
	if(finalStr == NULL)
	{
		fprintf(stderr, "Error while allocating memory.\n");
		exit(EXIT_FAILURE);
	}
	
	*finalStr = '\0';                                         /* Apenas inicializa finalStr, indicando que está vazia. */
	
	saveptr = finalStr;                                       /* Inicializa saveptr, apontando para o inicio da string final. */
	travelsTreeLeaf(tree, getFinalString, &saveptr);          /* Passa por todas as folhas da árvore, copiando suas informações para saveptr. */

	removeExtraCaracters(finalStr);                           /* Remove os caracteres indesejados da string final. */


	freeTree(&tree);                                          /* Libera a memória ocupada pela árvore. */
	
	free(productions->axiom);                                 /* Libera a memória ocupada pelo axioma. */
	freeList(&productions->rules, freeRule);                  /* Libera a memória ocupada pela lista de regras. */
	free(productions);                                        /* Libera a memória ocupada pela estrutura productions. */
	
	return finalStr;                                          /* Retorna a string final. */
}


/*
 * Executa a segunda e a terceira parte do trabalho, gerando uma string final. 
 * Esta string final será escrita no arquivo postscript de saída.
 * Retorna um ponteiro para a string final com os passos de tartaruga, de modo especificado na segunda e terceira parte do trabalho, em caso de
 * sucesso.
 * Retorna NULL em caso de o axioma for vazio, resultando em um arquivo sem passos, e em caso de algum erro.
 */
char* secondPart(char* str, double angle)
{
	Stack* stack;                      /* Ponteiro para pilha. */
	Turtle* turtle;                    /* Ponteiro para estrutura turtle. */
	
	
	/*
	 * Paleta de cores RGB. 
	 * Cada linha da matriz representa uma cor. 
	 * A primeira coluna é o red.
	 * A segunda coluna é o green.
	 * A terceira coluna é o blue.
	 */ /*                                  */
	unsigned char color[NUMCOLORS][3] = {{184, 135, 11}, {179, 138, 10}, {174, 141, 10}, {169, 144, 10}, \
{164, 147, 9}, {159, 151, 9}, {154, 154, 9}, {149, 157, 8}, \
{144, 160, 8}, {139, 164, 8}, {134, 167, 8}, {129, 170, 7}, \
{124, 173, 7}, {119, 177, 7}, {114, 180, 6}, {109, 183, 6}, \
{104, 186, 6}, {99, 190, 5}, {94, 193, 5}, {89, 196, 5}, \
{84, 199, 5}, {79, 203, 4}, {74, 206, 4}, {69, 209, 4}, \
{64, 212, 3}, {59, 216, 3}, {54, 219, 3}, {49, 222, 2}, \
{44, 225, 2}, {39, 229, 2}, {34, 232, 2}, {29, 235, 1}, \
{24, 238, 1}, {19, 242, 1}, {14, 245, 0}, {9, 248, 0}, \
{4, 251, 0}, {0, 255, 0}, {1, 250, 6}, {3, 246, 12}, \
{5, 242, 18}, {7, 238, 24}, {8, 234, 30}, {10, 230, 36}, \
{12, 226, 42}, {14, 222, 48}, {15, 218, 54}, {17, 214, 60}, \
{19, 210, 66}, {21, 206, 72}, {22, 202, 79}, {24, 198, 85}, \
{26, 194, 91}, {28, 190, 97}, {29, 186, 103}, {31, 182, 109}, \
{33, 177, 115}, {35, 173, 121}, {36, 169, 127}, {38, 165, 133}, \
{40, 161, 139}, {42, 157, 145}, {43, 153, 152}, {45, 149, 158}, \
{47, 145, 164}, {49, 141, 170}, {50, 137, 176}, {52, 133, 182}, \
{54, 129, 188}, {56, 125, 194}, {57, 121, 200}, {59, 117, 206}, \
{61, 113, 212}, {63, 109, 218}, {65, 105, 225}, {65, 102, 222}, \
{65, 99, 219}, {65, 96, 217}, {66, 93, 214}, {66, 90, 212}, \
{66, 87, 209}, {67, 85, 207}, {67, 82, 204}, {67, 79, 201}, \
{67, 76, 199}, {68, 73, 196}, {68, 70, 194}, {68, 68, 191}, \
{69, 65, 189}, {69, 62, 186}, {69, 59, 183}, {70, 56, 181}, \
{70, 53, 178}, {70, 51, 176}, {70, 48, 173}, {71, 45, 171}, \
{71, 42, 168}, {71, 39, 165}, {72, 36, 163}, {72, 34, 160}, \
{72, 31, 158}, {73, 28, 155}, {73, 25, 153}, {73, 22, 150}, \
{73, 19, 147}, {74, 17, 145}, {74, 14, 142}, {74, 11, 140}, \
{75, 8, 137}, {75, 5, 135}, {75, 2, 132}, {76, 0, 130}, \
{80, 3, 132}, {84, 7, 135}, {89, 10, 138}, {93, 14, 141}, \
{97, 17, 144}, {102, 21, 147}, {106, 24, 150}, {111, 28, 153}, \
{115, 31, 156}, {119, 35, 159}, {124, 38, 162}, {128, 42, 165}, \
{132, 45, 167}, {137, 49, 170}, {141, 52, 173}, {146, 56, 176}, \
{150, 59, 179}, {154, 63, 182}, {159, 66, 185}, {163, 70, 188}, \
{167, 73, 191}, {172, 77, 194}, {176, 80, 197}, {181, 84, 200}, \
{185, 87, 202}, {189, 91, 205}, {194, 94, 208}, {198, 98, 211}, \
{202, 101, 214}, {207, 105, 217}, {211, 108, 220}, {216, 112, 223}, \
{220, 115, 226}, {224, 119, 229}, {229, 122, 232}, {233, 126, 235}, \
{238, 130, 238}, {238, 126, 231}, {238, 122, 224}, {239, 119, 218}, \
{239, 115, 211}, {240, 111, 204}, {240, 108, 198}, {241, 104, 191}, \
{241, 101, 185}, {242, 97, 178}, {242, 93, 171}, {243, 90, 165}, \
{243, 86, 158}, {244, 83, 152}, {244, 79, 145}, {245, 75, 138}, \
{245, 72, 132}, {246, 68, 125}, {246, 65, 119}, {246, 61, 112}, \
{247, 57, 105}, {247, 54, 99}, {248, 50, 92}, {248, 46, 85}, \
{249, 43, 79}, {249, 39, 72}, {250, 36, 66}, {250, 32, 59}, \
{251, 28, 52}, {251, 25, 46}, {252, 21, 39}, {252, 18, 33}, \
{253, 14, 26}, {253, 10, 19}, {254, 7, 13}, {254, 3, 6}, \
{255, 0, 0}, {255, 4, 0}, {255, 9, 0}, {255, 13, 0}, \
{255, 18, 0}, {255, 23, 0}, {255, 27, 0}, {255, 32, 0}, \
{255, 36, 0}, {255, 41, 0}, {255, 46, 0}, {255, 50, 0}, \
{255, 55, 0}, {255, 59, 0}, {255, 64, 0}, {255, 69, 0}, \
{255, 73, 0}, {255, 78, 0}, {255, 83, 0}, {255, 87, 0}, \
{255, 92, 0}, {255, 96, 0}, {255, 101, 0}, {255, 106, 0}, \
{255, 110, 0}, {255, 115, 0}, {255, 119, 0}, {255, 124, 0}, \
{255, 129, 0}, {255, 133, 0}, {255, 138, 0}, {255, 142, 0}, \
{255, 147, 0}, {255, 152, 0}, {255, 156, 0}, {255, 161, 0}, \
{255, 166, 0}, {253, 165, 0}, {251, 164, 0}, {249, 163, 0}, \
{247, 162, 1}, {245, 161, 1}, {243, 160, 1}, {241, 159, 2}, \
{239, 159, 2}, {237, 158, 2}, {235, 157, 3}, {233, 156, 3}, \
{231, 155, 3}, {229, 154, 3}, {227, 153, 4}, {225, 153, 4}, \
{223, 152, 4}, {221, 151, 5}, {219, 150, 5}, {217, 149, 5}, \
{215, 148, 6}, {213, 147, 6}, {211, 147, 6}, {209, 146, 7}, \
{207, 145, 7}, {205, 144, 7}, {203, 143, 7}, {201, 142, 8}, \
{199, 141, 8}, {197, 141, 8}, {195, 140, 9}, {193, 139, 9}, \
{191, 138, 9}, {189, 137, 10}, {187, 136, 10}, {185, 135, 10}};
	
	double temp_x1, temp_y1;           /* Variáveis temporárias para as futuras posições da tartaruga. */

	char* finalStr;                    /* Ponteiro para a string final que será gerada. */
	char buff[128];	                   /* Buffer para armazenar temporariamente cada linha de comando. */

	int nInt;                          /* Variável temporária para ajudar na conversão de valores inteiros. */
	double nDouble;                    /* Variável temporária para ajudar na conversão de valores ponto flutuante. */

	size_t finalStrLength, i, j;       /* finalStrLength: Armazena a quantidade de bytes de memória alocada para finalStr. */
	                                   /* i: Índice de finalStr. */
	                                   /* j: Índice de buff. */
	
	stack = createEmptyStack();                                      /* Cria uma pilha vazia. */
	turtle = createTurtle(0.0, 0.0, 0.0, UNIT, 0, 1);                /* Cria uma estrutura turtle com as condições iniciais da tartaruga. */
	
	angle = toRadians(toDegrees(angle));                             /* Converte o valor de angle para radianos. */
	
	finalStrLength = BYTES;                                          /* Inicializa uma quantidade inicial de bytes para ser alocado. */
	finalStr = (char *) malloc(finalStrLength);                      /* Aloca a quantidade de bytes inicial para finalStr. */
	
	if(finalStr == NULL)
	{
		fprintf(stderr, "Error while allocating memory.\n");                           
		exit(EXIT_FAILURE); 												     
	}
	                                                     
	
	/* Configura a cor inicial. */
	/* WARNING: BYTES deve ser grande o suficiente para que finalStr possa comportar a string " %.4f %.4f %.4f setrgbcolor \n\0". */
	sprintf(finalStr, " %.4f %.4f %.4f setrgbcolor \n", rgbToPS(color[turtle->color][0]), rgbToPS(color[turtle->color][1]), rgbToPS(color[turtle->color][2]));

	i = strlen(finalStr);      /* Inicializa o índice da string final, para que os próximos comandos sejam concatenados a partir da posição i. */
	
	
	while(*str != '\0')        /* Enquanto a string de entrada não chegar ao fim. */
	{
		buff[0] = '\0';        /* Configura a primeira posição de buff para '\0' indicando que buff está vazio. */
		
		switch(*str)           /* Para cada caracter em str, é feita uma série de analises. */
		{
			case 'F': 
				temp_x1 = x1(turtle->x, turtle->orientation, turtle->length);      /* Gera a próxima posição da tartaruga no eixo x. */
				temp_y1 = y1(turtle->y, turtle->orientation, turtle->length);      /* Gera a próxima posição da tartaruga no eixo y. */
				sprintf(buff, " n %.4f %.4f m %.4f %.4f l s \n", turtle->x, turtle->y, temp_x1, temp_y1);  /* Escreve comando PS em buff. */
				turtle->x = temp_x1;                                               /* Atualiza a atual posição em x da tartaruga. */
				turtle->y = temp_y1;                                               /* Atualiza a atual posição em y da tartaruga. */
				str++;                                                             /* Incrementa ponteiro para str. */
				break;
	
			case 'G':
				turtle->x = x1(turtle->x, turtle->orientation, turtle->length);    /* Atualiza a posição em x da tartaruga. */
				turtle->y = y1(turtle->y, turtle->orientation, turtle->length);    /* Atualiza a posição em y da tartaruga. */
				str++;                                                             /* Incrementa ponteiro para str. */
				break;
	
			case '+':
				if(turtle->counterclockwise)                                       /* Caso a tartaruga girar em sentido anti-horário. */
					turtle->orientation += angle;                                  /* Adiciona angle à orientação da tartaruga. */
				else                                                               /* Caso contrário. */
					turtle->orientation -= angle;                                  /* Subtrai angle da orientação da tartaruga. */
				str++;                                                             /* Incrementa ponteiro para str. */
				break;
	
			case '-':
				if(turtle->counterclockwise)                                       /* Caso a tartaruga girar em sentido anti-horário. */
					turtle->orientation -= angle;                                  /* Subtrai angle da orientação da tartaruga. */
				else                                                               /* Caso contrário. */
					turtle->orientation += angle;                                  /* Adiciona angle à orientação da tartaruga. */
				str++;                                                             /* Incrementa ponteiro para str. */
				break;
	
			case '[':                 /* Cria uma estrutura turtle com as informações idênticas a turtle atual e a coloca na pilha. */
				push(&stack, createTurtle(turtle->x, turtle->y, turtle->orientation, turtle->length, turtle->color, turtle->counterclockwise));
				str++;                                                             /* Incrementa ponteiro para str. */
				break;
	
			case ']': 
				if(emptyStack(stack))                                              /* Caso a pilha estiver vazia. */
				{  														           /* Imprime mensagem de aviso. */
					fprintf(stdout, "Warning ! Number of [ and ] does not match. Nothing to restore. Using the current configuration.\n");
				}   
				else                                                               /* Caso contrário. */
				{  
					free(turtle);                                                  /* Libera a memória ocupada pela estrutura turtle corrente. */
					turtle = pop(&stack);                                          /* Retira estrutura turtle da pilha. */
					sprintf(buff, " %.4f %.4f %.4f setrgbcolor \n", rgbToPS(color[turtle->color][0]), rgbToPS(color[turtle->color][1]), rgbToPS(color[turtle->color][2]));
				}                                                                  /* Configura a cor com a turtle atual. */
				str++;                                                             /* Incrementa ponteiro para str. */
				break;
	
			case '>':
				nInt = 1;                                                   /* Inicializa nInt com 1, para o caso de não haver número após o >. */
				if(sscanf(++str, "%d", &nInt) == 1)                         /* Lê de str o próximo caracter. Se for inteiro, então */
				{	
					if(nInt < 0)                                            /* Se o número lido for negativo. */
					{
						fprintf(stdout, "At somewhere '>%d', n cannot be a negative number.\n", nInt);  /* Imprime erro. */
						freeStack(&stack);                                                              /* Libera memória ocupada pela pilha. */
						free(turtle);                                                                   /* Libera memória ocupada por turtle. */
						free(finalStr);                                                                 /* Libera memória ocupada por finalStr. */
						return NULL;                                                                    /* Retorna NULL. */
					}
					str += strspn(str, "0123456789");                                   /* Pula os caracteres que representam o número inteiro. */
				}
				turtle->color = colorIncrement(nInt, turtle->color);                    /* Incrementa a paleta de cor. */
				sprintf(buff, " %.4f %.4f %.4f setrgbcolor \n", rgbToPS(color[turtle->color][0]), rgbToPS(color[turtle->color][1]), rgbToPS(color[turtle->color][2]));
				break;                                                                  /* Configura a tartaruga para a cor incrementada. */
			
			case '<':
				nInt = 1;                                                   /* Inicializa nInt com 1, para o caso de não haver número após o <. */
				if(sscanf(++str, "%d", &nInt) == 1)                         /* Lê de str o próximo caracter. Se for inteiro, então */
				{	 
					if(nInt < 0)                                            /* Se o número lido for negativo. */
					{                                                       
						fprintf(stdout, "At somewhere '<%d', n cannot be a negative number.\n", nInt);  /* Imprime erro. */
						freeStack(&stack);                                                              /* Libera memória ocupada pela pilha. */
						free(turtle);                                                                   /* Libera memória ocupada por turtle. */
						free(finalStr);                                                                 /* Libera memória ocupada por finalStr. */
						return NULL;                                                                    /* Retorna NULL. */
					}
					str += strspn(str, "0123456789");                                   /* Pula os caracteres que representam o número inteiro. */
				}
				turtle->color = colorDecrement(nInt, turtle->color);                    /* Incrementa a paleta de cor. */
				sprintf(buff, " %.4f %.4f %.4f setrgbcolor \n", rgbToPS(color[turtle->color][0]), rgbToPS(color[turtle->color][1]), rgbToPS(color[turtle->color][2]));
				break;                                                                  /* Configura a tartaruga para a cor incrementada. */
			
			case 'c':
				nInt = 0;                               /* Caso o usuário não especificar um valor de para a cor, por padrão muda para a cor 0. */
				if(sscanf(++str, "%d", &nInt) == 1)     /* Lê de str o próximo caracter. Se for inteiro, então */
				{
					if(nInt < 0)                        /* Se o número lido for negativo. */
					{
						fprintf(stdout, "At somewhere 'c%d', n cannot be a negative number.\n", nInt);  /* Imprime erro. */
						freeStack(&stack);                                                              /* Libera memória ocupada pela pilha. */
						free(turtle);                                                                   /* Libera memória ocupada por turtle. */
						free(finalStr);                                                                 /* Libera memória ocupada por finalStr. */
						return NULL;                                                                    /* Retorna NULL. */ 
					}
					str += strspn(str, "0123456789");                                   /* Pula os caracteres que representam o número inteiro. */
				}
				turtle->color = colorNumber(nInt);                                      /* Muda a cor. */
				sprintf(buff, " %.4f %.4f %.4f setrgbcolor \n", rgbToPS(color[turtle->color][0]), rgbToPS(color[turtle->color][1]), rgbToPS(color[turtle->color][2]));
				break;                                                                  /* Configura a tartaruga para a nova cor. */
			
			case '@':
				if(*++str == 'I')                                                       /* Caso houver um 'I' após o '@'. */
				{
					if(*++str == 'Q')                                                   /* Caso houver um 'Q' após o 'I'. */
					{
						if(sscanf(++str, "%lf", &nDouble) == 1)                         /* Lê de str o próximo caracter. Se for double, então */
						{
							if(nDouble <= 0.0)                                           /* Se o número lido for negativo ou zero. */
							{
								fprintf(stdout, "At somewhere '@IQ%.2f', n cannot be zero or negative.\n", nDouble);  /* Imprime erro. */
								freeStack(&stack);                                               /* Libera memória ocupada pela pilha. */
								free(turtle);                                                    /* Libera memória ocupada por turtle. */
								free(finalStr);                                                  /* Libera memória ocupada por finalStr. */
								return NULL;                                                     /* Retorna NULL. */ 
							}
							str += strspn(str, ".0123456789");                          /* Pula os caracteres que representam o número double. */
							turtle->length *= 1.0 / sqrt(nDouble);                      /* Atualiza o novo comprimento da linha. */
						}
					}
					else if(sscanf(str, "%lf", &nDouble) == 1)                          /* Caso houver um double após o 'I'. */                  
					{                                                                   
						if(nDouble <= 0.0)                                              /* Se o número lido for negativo ou zero. */
						{
							fprintf(stdout, "At somewhere '@I%.2f', n cannot be zero or negative.\n", nDouble);  /* Imprime erro. */
							freeStack(&stack);                                               /* Libera memória ocupada pela pilha. */
							free(turtle);                                                    /* Libera memória ocupada por turtle. */
							free(finalStr);                                                  /* Libera memória ocupada por finalStr. */
							return NULL;                                                     /* Retorna NULL. */ 
						}
						str += strspn(str, ".0123456789");                              /* Pula os caracteres que representam o número double. */
						turtle->length *= 1.0 / nDouble;                                /* Atualiza o novo comprimento da linha. */
					}
				}
				else if(*str == 'Q')                                                    /* Caso houver um 'Q' após o '@'. */
				{                    
					if(*(++str) == 'I')                                                 /* Caso houver um 'I' após o 'Q'. */
					{
						if(sscanf(++str, "%lf", &nDouble) == 1)                         /* Lê de str o próximo caracter. Se for double, então */
						{                                                                      
							if(nDouble <= 0.0)                                          /* Se o número lido for negativo ou zero. */
							{
								fprintf(stdout, "At somewhere '@QI%.2f', n cannot be zero or negative.\n", nDouble);  /* Imprime erro. */
								freeStack(&stack);                                               /* Libera memória ocupada pela pilha. */
								free(turtle);                                                    /* Libera memória ocupada por turtle. */
								free(finalStr);                                                  /* Libera memória ocupada por finalStr. */
								return NULL;                                                     /* Retorna NULL. */ 
							}
							str += strspn(str, ".0123456789");                          /* Pula os caracteres que representam o número double. */
							turtle->length *= 1.0 / sqrt(nDouble);                      /* Atualiza o novo comprimento da linha. */
						}
					}
					else if(sscanf(str, "%lf", &nDouble) == 1)                          /* Caso houver um double após o 'Q'. */
					{
						if(nDouble < 0.0)                                               /* Se o número lido for negativo. */
						{
							fprintf(stdout, "At somewhere '@Q%.2f', n cannot be negative.\n", nDouble);           /* Imprime erro. */
							freeStack(&stack);                                               /* Libera memória ocupada pela pilha. */
							free(turtle);                                                    /* Libera memória ocupada por turtle. */
							free(finalStr);                                                  /* Libera memória ocupada por finalStr. */
							return NULL;                                                     /* Retorna NULL. */ 
						}
						str += strspn(str, ".0123456789");                              /* Pula os caracteres que representam o número double. */
						turtle->length *= sqrt(nDouble);                                /* Atualiza o novo comprimento da linha. */
					}
				}
				else if(sscanf(str, "%lf", &nDouble) == 1)                              /* Caso houver um double após o '@'. */
				{
					if(nDouble < 0.0)                                                   /* Se o número lido for negativo. */
					{
						fprintf(stdout, "At somewhere '@%.2f', n cannot be negative.\n", nDouble);           /* Imprime erro. */
						freeStack(&stack);                                              /* Libera memória ocupada pela pilha. */
						free(turtle);                                                   /* Libera memória ocupada por turtle. */
						free(finalStr);                                                 /* Libera memória ocupada por finalStr. */
						return NULL;                                                    /* Retorna NULL. */ 
					}
					str += strspn(str, ".0123456789");                                  /* Pula os caracteres que representam o número double. */
					turtle->length *= nDouble;                                          /* Atualiza o novo comprimento da linha. */
				}
				break;
			
			case '!':
				if(turtle->counterclockwise)                                 /* Caso a tartaruga estiver girando em sentido anti-horário. */
					turtle->counterclockwise = 0;                            /* Muda o sentido da tartaruga para girar em sentido horário. */
				else                                                         /* Caso contrário. */
					turtle->counterclockwise = 1;                            /* Muda o sentido da tartaruga para girar em sentido anti-horário. */
				str++;                                                       /* Incrementa ponteiro para str. */
				break;
				
			case '|':
				turtle->orientation += M_PI;                                 /* Adiciona PI à orientação, fazendo a tartaruga girar 180 graus. */
				str++;                                                       /* Incrementa ponteiro para str. */
				break;
			
			default:
				fprintf(stdout, "Unknown keyword or command '%c'.\n", *str); /* Imprime erro. */
				freeStack(&stack);                                           /* Libera memória ocupada pela pilha. */
				free(turtle);                                                /* Libera memória ocupada por turtle. */
				free(finalStr);                                              /* Libera memória ocupada por finalStr. */
				return NULL;                                                 /* Retorna NULL. */ 
		}
	
	
		/* Adiciona conteúdo de buff à finalStr e verifica se finalStr está acabando seu espaço. Se sim, realloca com um espaço maior. */
		for(j = 0; buff[j] != '\0'; j++)                                     /* Enquanto não chegar ao fim de buff. */
		{
			finalStr[i++] = buff[j];                                         /* finalStr[i] recebe buff[j]. Incrementa i. */
			
			if(i == finalStrLength)                                          /* Caso preencher todo o espaço de finalStr. */
			{
				finalStrLength += NEWBYTES;                                  /* Adiciona NEWBYTES ao tamanho de finalStr. */
				
				finalStr = (char *) realloc(finalStr, finalStrLength);       /* Realloca o espaço de finalStr com o novo tamanho. */
				if(finalStr == NULL)                                         /* Caso houver erro de realocação. */
				{
					fprintf(stderr, "Error while allocating memory.\n");     /* Imprime erro. */
					exit(EXIT_FAILURE); 									 /* Aborta o programa retornando EXIT_FAILURE. */
				}
			}
		}
	}
	
	finalStr[i] = '\0';                                                      /* Coloca o terminador de string '\0' em finalStr. */
	
	freeStack(&stack);                                                       /* Libera a pilha. */
	free(turtle);                                                            /* Libera a memória ocupada pela estrutura turtle corrente. */
	
	return finalStr;                                                         /* Retorna o ponteiro para a string final. */
}


int colorIncrement(int n, int value)                                         /* Incrementa a cor atual para uma nova cor da paleta de cores. */
{	
	return (value + n) % NUMCOLORS;                                /* Retorna o valor atual da paleta acrescido de n e mapeado em um */
}                                                                  /* valor de 0 a NUMCOLORS - 1, pois NUMCOLORS é o número de cores na paleta. */


int colorDecrement(int n, int value)                                         /* Decrementa a cor atual para uma nova cor da pelata de cores. */
{
	while(n > 0)                                                             /* Enquanto houver o que decrementar. */
	{
		n--;                                                                 /* Decrementa o contador. */ 
		if(value == 0)                                                       /* Caso o valor for zero. */
			value = NUMCOLORS - 1;                                           /* Valor recebe NUMCOLORS - 1, ou seja, valor gira  */ 
			                                                                 /* de NUMCOLORS - 1 a 0.                            */
		else                                                                 /* Caso contrário. */
			value--;                                                         /* Decrementa valor. */
	}
	
	return value;                                                            /* Retorna valor decrementado. */
}


void removeExtraCaracters(char* str)                                         /* Filtra a string str, removendo caracteres que não estão em */
{                                                                            /* charToSecondPart.                                          */
	int i, j;                                                                /* i: Índice de str original. */
	                                                                         /* j: Índice de str nova, a que estará filtrada. */
	i = j = 0;                                                               /* Inicializa i e j. */
	
	while(str[i] != '\0')                                                    /* Enquanto não chegar ao fim de str. */
	{
		if(strchr(charToSecondPart, str[i]) != NULL)                         /* Se str[i] estiver em charToSecondPart. */
			str[j++] = str[i];                                               /* Armazena str[i] na posição str[j] e incrementa j. */
			
		i++;                                                                 /* Incrementa i. */
	}
	
	str[j] = '\0';                                                           /* Coloca terminador de string '\0' na posição str[j]. */
}











