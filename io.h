#ifndef __IO__
#define __IO__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TAM_LINHA 1025

typedef struct arqCsv {
	FILE *arquivo;
	unsigned long numLinhas;
	unsigned long numColunas;
	unsigned short *sizes;
	int *tipo;
	char ***matriz;
	char ***matrizAux;
}csv_t;

typedef struct estatisticas {
	unsigned long contador;
	float media;
	float desvio;
	float mediana;
	char *moda;
	int contModa;
	char *max;
	char *min;
	char **vetor;
}estat_t;

typedef struct variavel {
	char *string;
	unsigned long pos;
}var_t;


//Menu do programa
void menu();


//Funcoes que fazem o pre-processamento do arquivp
csv_t* abreCsv(char *path);
csv_t* processaCsv(csv_t *csv);
int copiaParaMatriz(csv_t *csv);


//Funcoes principais do programa
void sumario(csv_t *csv);
int mostrar(csv_t *csv, char ***matriz, unsigned long numLin, unsigned long numCol, unsigned long *posicoes);
int filtro(csv_t *csv, int (*compara)(char *word, char *valor, char *filtro, int tipo));
int descricao(csv_t *csv);
int ordenacao(csv_t *csv);
int selecao(csv_t *csv);
int dadosFaltantes(csv_t *csv);
int exibeNaN(csv_t *csv);
void substituiPelaMedia(csv_t *csv);
void substituiPeloProx(csv_t *csv);
int removeNaN(csv_t *csv);
int salvaDados(csv_t *csv, char ***matriz, int numLin, int numCol);


//Funcoes de impressao
int imprimeMatriz(csv_t *csv, char ***matriz, int minLin, int maxLin, int minCol, int maxCol, unsigned long *posicoes);
int imprimeFormatado(char *string, int tamMaior, int indice);


//Funcoes de comparacao
int comparaIgual(char *word, char *valor, int tipo);
int comparaMaior(char *word, char *valor, int tipo);
int comparaMaiorIgual(char *word, char *valor, int tipo);
int comparaMenor(char *word, char *valor, int tipo);
int comparaMenorIgual(char *word, char *valor, int tipo);
int comparaDiff(char *word, char *valor, int tipo);
int compara(char *word, char *valor, char *filtro, int tipo);

//Funcoes de ordenacao
void sort(var_t *linha, int count, int tipo);
int compString(const void *str1, const void *str2);
int compFloat(const void *elem1, const void *elem2);
void selectionSort(unsigned long *v, unsigned long contLin);

//Outras funcoes
int buscaPos(csv_t *csv, char *var);
int descartaOriginal(csv_t *csv, int numLin, int numCol);
void troca(unsigned long *v, int a, int b);
int buscaNaNv2(csv_t *csv, char **v, unsigned long linha, int numCol);
int buscaNaN(csv_t *csv, int linha);
void menuDadosFaltantes();
float calcMedia(csv_t *csv, char ***matriz, int col, int numLin);
void getVar(csv_t *csv, char* var);
void getFiltro(csv_t *csv, char* filtro);
void getVariaveis(csv_t *csv, char *var);
char getOrdem();
char getOp(int tipo);

//Funcoes que liberam toda a memoria restante
void liberaMatriz(csv_t *csv, char ***matriz, int numLin, int numCol);
void liberaCsv(csv_t *csv);

#endif
