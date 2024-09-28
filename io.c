#include "io.h"

//Menu que mostra as opcoes de funcoes para o usuario
void menu() {

	printf("1) Sumario do arquivo\n");
	printf("2) Mostrar\n");
	printf("3) Filtros\n");
	printf("4) Descricao dos dados\n");
	printf("5) Ordenacao\n");
	printf("6) Selecao\n");
	printf("7) Dados Faltantes\n");
	printf("8) Salvar Dados\n");
	printf("9) Fim\n");
	printf("Digite sua opcao: ");

}

csv_t* abreCsv(char *path) {

	csv_t *csv;

	//Se nao conseguir alocar mem para a struct, retorna NULL
	if ((csv = (csv_t*) malloc(sizeof(csv_t))) == NULL) {
		printf("Falha em alocar memoria\n");
		return NULL;
	}

	//Se nao conseguir abrir o arquivo, retorna NULL
	if ((csv->arquivo = fopen(path, "r")) == NULL) {
		printf("Falha em abrir o arquivo\n");
		return NULL;
	}

	//Inicializa todas as variaveis da struct
	csv->matriz = NULL;
	csv->sizes = NULL;
	csv->tipo = NULL;
	csv->numLinhas = 0;
	csv->numColunas = 0;

	if ((csv = processaCsv(csv)) == NULL) {
		printf("Falha em alocar memoria\n");
		return NULL;
	}

	if (copiaParaMatriz(csv) == 0) {
		printf("Falha em alocar memoria\n");
		return NULL;
	}

	return csv;

}

//Preenche a maior parte das variaveis da struct
csv_t* processaCsv(csv_t *csv) {

	char linha[TAM_LINHA], *buffer, *word;
	int tipo = 0, contTipo = 0, i;

	while (fgets(linha, TAM_LINHA, csv->arquivo) != NULL) {

		buffer = linha;
		word = strsep(&buffer,",");

		//Analiza a primeira linha para determinar a quantidade de colunas
		if (csv->numLinhas == 0) {
			while (word != NULL) {
				csv->numColunas++;
				word = strsep(&buffer, ",");
			}
			//Aloca dois vetores zerados baseado no numero de colunas
			if ((csv->sizes = (unsigned short*) calloc(csv->numColunas,sizeof(unsigned short))) == NULL)
				return NULL;
			if ((csv->tipo = (int*) calloc(csv->numColunas,sizeof(int))) == NULL)
				return NULL;
		}
		else {

			i = 0;
			while (word != NULL) {

				//Compara o tam de uma palavra de cada coluna com um int em csv->sizes 
				//na posicao relativa a mesma coluna, e, caso seja maior, ele o substituira
				if (strlen(word) > csv->sizes[i])
					csv->sizes[i] = strlen(word);

				//Enquanto tipo = 0, determina o tipo da string de cada coluna, tipo = 1 quando
				//verificar o tipo das variaveis de uma linha completa
				if (!tipo) {
					if (strlen(word) == 0)
						contTipo--;
					else if (atof(word) == 0) {
						csv->tipo[i] = 0;
						contTipo++;
					}
					else if (atof(word) != 0) {
						csv->tipo[i] = 1;
						contTipo++;
					}
				}
				
				word = strsep(&buffer, ",");
				i++;

			}

			if (contTipo == csv->numColunas)
				tipo = 1;
		}

		csv->numLinhas++;
	}

	rewind(csv->arquivo);

	//Confere o tamanho das strings na primeira linha do csv
	i = 0;
	fgets(linha, TAM_LINHA, csv->arquivo);
	buffer = linha;
	word = strsep(&buffer,",");
	while (word != NULL) {
		if (strlen(word) > csv->sizes[i])
			csv->sizes[i] = strlen(word);
		word = strsep(&buffer,",");
		i++;
	}

	rewind(csv->arquivo);

	return csv;

}

int copiaParaMatriz(csv_t *csv) {
	
	char linha[TAM_LINHA], *buffer, *word;
	
	//Aloca uma matriz csv->numLinhas x csv->numColunas
	if ((csv->matriz = (char***) malloc(csv->numLinhas * sizeof(char**))) == NULL)
		return 0;
	for (int i = 0; i < csv->numLinhas; i++)
		if ((csv->matriz[i] = (char**) malloc(csv->numColunas * sizeof(char*))) == NULL)
			return 0;

	//Copia todas as strings do csv para a matriz
	for (int i = 0; i < csv->numLinhas; i++) {
		fgets(linha, TAM_LINHA, csv->arquivo);
		buffer = linha;
		buffer[strcspn(buffer, "\n")] = 0; //Substitui \n por \0 para nao pular uma linha	
		for (int j = 0; j < csv->numColunas; j++) {
			word = strsep(&buffer, ",");
			if (word != NULL) {
				if (strlen(word) == 0)
					word = "NaN";
				csv->matriz[i][j] = strdup(word);
			}
		}
	}

	rewind(csv->arquivo);
	return 1;

}

void sumario(csv_t *csv) {

	//Percorre a primeira linha e imprime o nome das variaveis
	for (int j = 0; j < csv->numColunas; j++) {
		if (csv->tipo[j] == 0) //Se csv->tipo[j] = 0, o tipo eh String
			printf("%s [S]\n", csv->matriz[0][j]);
		else
			printf("%s [N]\n", csv->matriz[0][j]); //Caso contrario, o tipo eh Numeric
	}
	printf("%ld variaveis encontradas\n", csv->numColunas);
}


int mostrar(csv_t *csv, char ***matriz, unsigned long numLin, unsigned long numCol, unsigned long *posicoes) {

	//Imprime todas as linhas caso numLinhas < 10
	if (numLin < 10) {
		if ((imprimeMatriz(csv,matriz,0,numLin,0,numCol,posicoes)) == 0)
			return 0;
	}
	else {
		if ((imprimeMatriz(csv,matriz,0,6,0,numCol,posicoes)) == 0) //imprime as 5 primeiras linhas
			return 0;
		if ((imprimeMatriz(csv,matriz,numLin-5,numLin,0,numCol,posicoes)) == 0)  //imprime as 5 ultimas linhas
			return 0;
	}

	printf("\n[%ld rows x %ld columns]\n", numLin-1, numCol);

	return 1;
}

int filtro(csv_t *csv, int (*compara)(char *word, char *valor, char *filtro, int tipo)) {

	int posVar = -1, iVet = 5;
	unsigned long contLin = 1, pos[10];
	char var[TAM_LINHA], filtro[TAM_LINHA], valor[TAM_LINHA];

	getVar(csv,var);
	getFiltro(csv,filtro);

	printf("Digite um valor: ");
	scanf("%s", valor);
	getchar();

	posVar = buscaPos(csv,var);

	if ((csv->matrizAux = (char***) malloc(csv->numLinhas * sizeof(char**))) == NULL)
		return 0;

	//Aloca e preenche, na matrizAux, uma linha contendo o nome das variaveis
	if ((csv->matrizAux[0] = (char**) malloc(csv->numColunas * sizeof(char*))) == NULL)
		return 0; 
	for (int j = 0; j < csv->numColunas; j++) {
		csv->matrizAux[0][j] = strdup(csv->matriz[0][j]);
	}

	//Compara todas as palavras da coluna desejada com o valor, se compara = 1,
	//copia a linha para a matrizAux
	for (int i = 1; i < csv->numLinhas; i++) {
		if (compara(csv->matriz[i][posVar],valor,filtro,csv->tipo[posVar]) == 1) {
			if ((csv->matrizAux[contLin] = (char**) malloc(csv->numColunas * sizeof(char*))) == NULL)
				return 0;
			for (int j = 0; j < csv->numColunas; j++)
				csv->matrizAux[contLin][j] = strdup(csv->matriz[i][j]);
			if (contLin < 6)
				pos[contLin-1] = i-1;
			else {
				pos[iVet] = i - 1;
				iVet++;
				if (iVet > 9)
					iVet = 5;
			}
			contLin++;
		}	   
	}

	if (contLin != 1) {
		if (contLin >= 10)
			selectionSort(pos,contLin);
		if ((mostrar(csv,csv->matrizAux,contLin,csv->numColunas,pos)) == 0)
			return 0;
		if ((descartaOriginal(csv,contLin,csv->numColunas)) == 0)
			return 0;
	}
	else
		printf("Nenhum dado que se encaixa no filtro foi encontrado\n");

	return 1;

}

int descricao(csv_t *csv) {

	estat_t *estat;
	int posVar = -1, modaCont = 0;
	char *modaNum, var[TAM_LINHA];

	//Aloca a struct estat e inicializa seus valores
	if ((estat = (estat_t*) malloc(sizeof(estat_t))) == NULL)
		return 0;
	estat->contModa = 0;
	estat->contador = 0;
	estat->mediana = 0;
	estat->desvio = 0;
	estat->media = 0;
	estat->moda = NULL;

	getVar(csv,var);

	posVar = buscaPos(csv,var);

	//Conta a quantidade de valores e calcula sua soma
	for (int i = 1; i < csv->numLinhas; i++)
		if (strcmp(csv->matriz[i][posVar],"NaN") != 0) {
			(estat->contador)++;  //Conta a quantidade de itens
			if (csv->tipo[posVar] == 1)
				estat->media += atof(csv->matriz[i][posVar]); //Acumula a soma dos valores
		}

	//Calcula a media
	estat->media /= estat->contador;

	//Insere em um vetor, todos os valores validos da coluna desejada
	var_t *vetor;
	if ((vetor = (var_t*) malloc(estat->contador*sizeof(var_t))) == NULL)
		return 0;
	int pos = 0;
	for (int i = 1; i < csv->numLinhas; i++) {//23 NaN NaN 41 58 29 42
		if (strcmp(csv->matriz[i][posVar],"NaN") != 0) {
			vetor[pos].string = csv->matriz[i][posVar];
			pos++;
		}
	}

	//Ordena o vetor
	sort(vetor,estat->contador,csv->tipo[posVar]);

	//O primeiro valor do vetor eh o menor, e o ultimo eh o maior
	estat->min = strdup(vetor[0].string);
	estat->max = strdup(vetor[estat->contador-1].string);

	//Calculo da moda
	modaNum = strdup(vetor[0].string);
	modaCont = 1;
	for (int i = 1; i < estat->contador; i++) {
		if (comparaIgual(vetor[i].string,modaNum,csv->tipo[posVar]) == 1)
			modaCont++;
		else {
			if (modaCont > estat->contModa) {
				estat->contModa = modaCont;
				if (estat->moda != NULL) 
					free(estat->moda);
				estat->moda = strdup(modaNum);
			}
			modaCont = 1;
			free(modaNum);
			modaNum = strdup(vetor[i].string);
		}

	}

	free(modaNum);

	//Executa somente para tipos numericos
	if (csv->tipo[posVar] == 1) {
		//Calcula a mediana
		if ((estat->contador % 2) != 0)
			estat->mediana = atof(vetor[estat->contador/2].string);
		else
			estat->mediana = (atof(vetor[((estat->contador-1)/2)].string) + atof(vetor[((estat->contador-1)/2)+1].string))/2;

		//Calcula o desvio padrao
		int sub = 0;
		for (int i = 0; i < estat->contador; i++) {
			sub += atof(vetor[i].string) - estat->media;
			estat->desvio += sub*sub;
			sub = 0;
		}
		estat->desvio = sqrt(estat->desvio/estat->contador);
	}

	printf("Contador: %ld\n", estat->contador);
	if (csv->tipo[posVar] == 1) {
		printf("Media: %.2f\n", estat->media);
		printf("Desvio: %.2f\n", estat->desvio);
		printf("Mediana: %.2f\n", estat->mediana);
	}
	printf("Moda: %s %d vezes\n", estat->moda, estat->contModa);
	if (csv->tipo[posVar] == 1) {
		printf("Min.: %s\n", estat->min);
		printf("Max.: %s\n", estat->max);
	}
	printf("Valores unicos: [");
	for (int i = 0; i < estat->contador; i++) {
		if (i == 0)
			printf("%s", vetor[i].string);
		else  {
			if (comparaDiff(vetor[i].string,vetor[i-1].string,csv->tipo[posVar]) == 1) {
				printf(", %s", vetor[i].string);
			}
		}
	}
	printf("]\n");

	free(vetor);
	free(estat->min);
	free(estat->max);
	free(estat->moda);
	free(estat);

	return 1;
}

int ordenacao(csv_t *csv) {

	int posVar;
	var_t *vetor;
	char var[TAM_LINHA], op;
	unsigned long v[10];;

	getVar(csv,var);
	getchar();
	op = getOrdem();
	getchar();

	posVar = buscaPos(csv,var);

	//Cria um vetor de string e armazena nele todos os elementos validos
	if ((vetor = (var_t*) malloc((csv->numLinhas-1)*sizeof(var_t))) == NULL)
		return 0;
	
	int pos = 0;
	for (int i = 1; i < csv->numLinhas; i ++) {
		vetor[pos].string = csv->matriz[i][posVar];
		vetor[pos].pos = i;
		pos++;
	}

	sort(vetor,csv->numLinhas-1,csv->tipo[posVar]);

	if ((csv->matrizAux = (char***) malloc(csv->numLinhas * sizeof(char**))) == NULL)
		return 0;

	//Aloca e preenche, na matrizAux, uma linha contendo o nome das variaveis
	if ((csv->matrizAux[0] = (char**) malloc(csv->numColunas * sizeof(char*))) == NULL)
		return 0;
	for (int j = 0; j < csv->numColunas; j++) 
		csv->matrizAux[0][j] = strdup(csv->matriz[0][j]);

	int iVet;
	//Preenche a matriz auxiliar em ordem crescente ou decrescente
	if (op == 'A')
		iVet = 0;
	else if (op == 'D')
		iVet = csv->numLinhas - 2;
	for (int i = 1; i < csv->numLinhas; i++) {
		if ((csv->matrizAux[i] = (char**) malloc(csv->numColunas * sizeof(char*))) == NULL)
			return 0;
		for (int j = 0; j < csv->numColunas; j++)
			csv->matrizAux[i][j] = strdup(csv->matriz[vetor[iVet].pos][j]);
		if (op == 'A') iVet++;
		else if (op == 'D') iVet--;
	}

	int lim = 5;
	//Copia, para um vetor, o indice das linhas que serao as 5 primeiras, e as 5 ultimas	
	if (op == 'A') {
		if (csv->numLinhas < 5) 
			lim = csv->numLinhas - 1;
		iVet = csv->numLinhas - 6;
		for (int i = 0; i < lim; i++)
			v[i] = vetor[i].pos-1;
		lim = 10;
		if (csv->numLinhas < 10) {
			lim = csv->numLinhas-1;
			iVet = 5;
		}
		for (int i = 5; i < lim; i++) {
			v[i] = vetor[iVet].pos-1;
			iVet++;
		}
	}
	else if (op == 'D') {
		lim = 5;
		if (csv->numLinhas < 5)
			lim = csv->numLinhas -1;
		iVet = csv->numLinhas - 2;
		for (int i = 0; i < lim; i++) {
			v[i] = vetor[iVet].pos-1;
			iVet--;
		}
		iVet = 4;
		if (csv->numLinhas < 10) {
			lim = csv->numLinhas - 1;
			iVet = csv->numLinhas - 7;
		}
		for (int i = 5; i < lim; i++) {
			v[i] = vetor[iVet].pos-1;
			iVet--;
		}
	}

	free(vetor);

	if ((mostrar(csv,csv->matrizAux,csv->numLinhas,csv->numColunas,v)) == 0)
		return 0;
	if ((descartaOriginal(csv,csv->numLinhas,csv->numColunas)) == 0)
		return 0;

	return 1;
}

int selecao(csv_t *csv) {

	char var[TAM_LINHA], **vetor, *word, *buffer, op;

	getchar();
	getVariaveis(csv,var);

	if ((vetor = (char**) malloc(csv->numColunas*sizeof(char*))) == NULL)
		return 0;

	unsigned long contCol = 0;
	buffer = var;
	word = strsep(&buffer," ");
	while (word != NULL) {
		vetor[contCol] = word;
		word = strsep(&buffer," ");
		contCol++;
	}

	if ((csv->matrizAux = (char***) malloc(csv->numLinhas * sizeof(char**))) == NULL)
		return 0;

	//Aloca e preenche, na matrizAux, uma linha contendo o nome das variaveis
	if ((csv->matrizAux[0] = (char**) malloc(contCol * sizeof(char*))) == NULL)
		return 0;
	for (int j = 0; j < contCol; j++) 
		csv->matrizAux[0][j] = strdup(csv->matriz[0][buscaPos(csv,vetor[j])]);


	for (int i = 1; i < csv->numLinhas; i++) {
		csv->matrizAux[i] = (char**) malloc(contCol * sizeof(char*)); 
		for (int j = 0; j < contCol; j++) {
			csv->matrizAux[i][j] = strdup(csv->matriz[i][buscaPos(csv,vetor[j])]);
		}
	}

	if ((mostrar(csv,csv->matrizAux,csv->numLinhas,contCol,NULL)) == 0)
		return 0;

	op = getOp(0);
	if (op == 'S') { //Se op == S cria um novo arquivo com os dados da matrizAux
		if ((salvaDados(csv,csv->matrizAux,csv->numLinhas,contCol)) == 0)
			return 0;
	}

	liberaMatriz(csv,csv->matrizAux,csv->numLinhas,contCol);

	free(vetor);

	return 1;
}

int dadosFaltantes(csv_t *csv) {

	int op;

	getchar();
	menuDadosFaltantes();
	scanf("%d", &op);

	while (op != 5) {
		switch(op) {
			case 1:
				if ((exibeNaN(csv)) == 0)
					return 0;
				break;
			case 2:
				substituiPelaMedia(csv);
				break;
			case 3:
				substituiPeloProx(csv);
				break;
			case 4:
				if ((removeNaN(csv)) == 0)
					return 0;
				break;
			default:
				printf("Digite uma opcao valida\n");
				break;
		}
		getchar();
		menuDadosFaltantes();
		scanf("%d", &op);

	}

	return 1;

}

int exibeNaN(csv_t *csv) {

	int iVet = 5;
	unsigned long contLin = 1, pos[10];

	getchar();

	if ((csv->matrizAux = (char***) malloc(csv->numLinhas * sizeof(char**))) == NULL)
		return 0;

	//Aloca e preenche, na matrizAux, uma linha contendo o nome das variaveis
	if ((csv->matrizAux[0] = (char**) malloc(csv->numColunas * sizeof(char*))) == NULL)
		return 0;
	for (int j = 0; j < csv->numColunas; j++) 
		csv->matrizAux[0][j] = strdup(csv->matriz[0][j]);

	for (int i = 1; i < csv->numLinhas; i++) {
		if (buscaNaN(csv,i) == 1) {
			if ((csv->matrizAux[contLin] = (char**) malloc(csv->numColunas * sizeof(char*))) == NULL)
				return 0;
			for (int j = 0; j < csv->numColunas; j++) 
				csv->matrizAux[contLin][j] = strdup(csv->matriz[i][j]);
			if (contLin < 6)
				pos[contLin-1] = i-1;
			else {
				pos[iVet] = i - 1;
				iVet++;
				if (iVet > 9)
					iVet = 5;
			}
			contLin++;

		}
	}
	if (contLin == 1) {
		printf("Nenhum NaN encontrado\n");		
		for (int j = 0; j < csv->numColunas; j++)
			free(csv->matrizAux[0][j]);
		free(csv->matrizAux[0]);
		free(csv->matrizAux);
		return 1;
	}

	printf("\n");
	selectionSort(pos,contLin);
	if ((mostrar(csv,csv->matrizAux,contLin,csv->numColunas,pos)) == 0)
		return 0;
	if ((descartaOriginal(csv,contLin,csv->numColunas)) == 0)
		return 0;

	return 1;

}

void substituiPelaMedia(csv_t *csv) {

	for (int j = 0; j < csv->numColunas; j++) {
		if (csv->tipo[j] == 1) {
			char media[csv->sizes[j]];
			snprintf(media,sizeof(media),"%.1f",calcMedia(csv,csv->matriz,j,csv->numLinhas));
			for (int i = 1; i < csv->numLinhas; i++) {
				if (strcmp(csv->matriz[i][j],"NaN") == 0) {
					free(csv->matriz[i][j]);
					csv->matriz[i][j] = strdup(media);
				}
			}
		}
	}

}

void substituiPeloProx(csv_t *csv) {

	for (int j = 0; j < csv->numColunas; j++) {
		if (csv->tipo[j] == 1) {
			for (int i = 1; i < csv->numLinhas; i++) {
				if ((strcmp(csv->matriz[i][j],"NaN") == 0) && (i < csv->numLinhas-1)) {
					if (strcmp(csv->matriz[i+1][j], "NaN") != 0) {
						free(csv->matriz[i][j]);
						csv->matriz[i][j] = strdup(csv->matriz[i+1][j]);
					}
					else if (strcmp(csv->matriz[i+1][j],"NaN") == 0) {
						int a = 2;
						while ((a <= csv->numLinhas - i - 1) && (strcmp(csv->matriz[i+a][j], "NaN") == 0)) 
							a++;
						if (a <= csv->numLinhas-1-i){
							free(csv->matriz[i][j]);
							csv->matriz[i][j] = strdup(csv->matriz[i+a][j]);
						}

					}
				}
			}
		}
	}
}

int removeNaN(csv_t *csv) {

	unsigned long contLin = 1;
	char var[TAM_LINHA], **vetor, *word, *buffer;

	getchar();
	getVariaveis(csv,var);	

	vetor = (char**) malloc(csv->numColunas*sizeof(char*));
	unsigned long contCol = 0;
	buffer = var;
	word = strsep(&buffer," ");
	while (word != NULL) {
		vetor[contCol] = word;
		word = strsep(&buffer," ");
		contCol++;
	}

	if ((csv->matrizAux = (char***) malloc(csv->numLinhas * sizeof(char**))) == NULL)
			return 0;

	//Aloca e preenche, na matrizAux, uma linha contendo o nome das variaveis
	if ((csv->matrizAux[0] = (char**) malloc(csv->numColunas * sizeof(char*))) == NULL)
		return 0;
	for (int j = 0; j < csv->numColunas; j++) 
		csv->matrizAux[0][j] = strdup(csv->matriz[0][j]);

	for (int i = 1; i < csv->numLinhas; i++) {
		if (buscaNaNv2(csv,vetor,i,contCol) == 0) {
			if ((csv->matrizAux[contLin] = (char**) malloc(csv->numColunas * sizeof(char*))) == NULL)
				return 0;
			for (int j = 0; j < csv->numColunas; j++) 
				csv->matrizAux[contLin][j] = strdup(csv->matriz[i][j]);
			contLin++;
		}
	}
	liberaMatriz(csv,csv->matriz,csv->numLinhas,csv->numColunas);
	if ((csv->matriz = (char***) malloc(contLin * sizeof(char**))) == NULL)
		return 0;
	for (int i = 0; i < contLin; i++) {
		if ((csv->matriz[i] = (char**) malloc(csv->numColunas * sizeof(char*))) == NULL)
			return 0;
		for (int j = 0; j < csv->numColunas; j++)
			csv->matriz[i][j] = strdup(csv->matrizAux[i][j]);
	}
	free(vetor);
	liberaMatriz(csv,csv->matrizAux,contLin,csv->numColunas);
	csv->numLinhas = contLin;
	printf("Pressione ENTER para continuar\n");

	return 1;

}

char getOp(int tipo) {

	char op;
	int sucesso = 0;

	while (sucesso == 0) {
		if (tipo == 0)
			printf("Deseja guardar um arquivo com os dados alterados? [S|N]: ");
		else
			printf("Deseja descartar os dados originais? [S|N]: ");
		scanf("%c", &op);
		if ((op == 'S') || (op == 'N'))
			sucesso = 1;
		else
			getchar();
	}

	return op;

}
char getOrdem() {

	char op;
	int sucesso = 0;

	while (sucesso == 0) {
		printf("Selecione uma opcao [A]scendente ou [D]ecrescente: ");
		scanf("%c", &op);
		if ((op == 'A') || (op == 'D'))
			sucesso = 1;
		else
			getchar();
	}

	return op;
}

void getVar(csv_t *csv, char* var) {
	
	int sucesso = 0;

	while (sucesso == 0) {
		printf("Entre com a variavel: ");
		scanf("%s", var);
		if (buscaPos(csv,var) != -1)
			sucesso = 1;
	}

}

void getFiltro(csv_t *csv, char* filtro) {
	
	int sucesso = 0;

	while (sucesso == 0) {
		printf("Escolha um filtro ( == > >= < <= != ): ");
		scanf("%s", filtro);
		if ((strcmp(filtro,"==") == 0)|| (strcmp(filtro,">") == 0))
			sucesso = 1;
		else if ((strcmp(filtro,">=") == 0)|| (strcmp(filtro,"<") == 0))
			sucesso = 1;
		else if ((strcmp(filtro,"<=") == 0)|| (strcmp(filtro,"!=") == 0))
			sucesso = 1;
	}

}

void getVariaveis(csv_t *csv, char *var) {
	
	char buffer[TAM_LINHA], *word, *linha;
	int sucesso = 0, cont;

	while (sucesso == 0) {
		printf("Entre com as variaveis que deseja selecionar (separadas por espaco): ");
		scanf("%[^\n]", var);
		getchar();
		strcpy(buffer,var);
		linha = buffer;
		cont = 0;
		word = strsep(&linha," ");
		while ((word != NULL) && (cont <= csv->numColunas)) {// 1 2 3 4
			if (buscaPos(csv,word) != -1)
				sucesso = 1;
			else {
				sucesso = 0;
				break;
			}
			cont++;
			word = strsep(&linha," ");
		}

	}

}

int salvaDados(csv_t *csv, char ***matriz, int numLin, int numCol) {
	
	char arqNome[TAM_LINHA];
	FILE* arqNovo;

	printf("Entre com o nome do arquivo: ");
	scanf("%s",arqNome);
	getchar();

	if ((arqNovo = fopen(arqNome,"w")) == NULL)
		return 0;

	for (int i = 0; i < numLin; i++) {
		for (int j = 0; j < numCol; j++) {
			if (strcmp(matriz[i][j],"NaN") != 0) //Caso o valor seja NaN, guarda ,,
				fputs(matriz[i][j],arqNovo);
			if (j != numCol-1) //Na ultima coluna nao imprime ,
				fprintf(arqNovo, ",");

		}
		fprintf(arqNovo, "\n"); //Pula para a proxima linha
	}

	printf("Arquivo guardado com sucesso\n");

	fclose(arqNovo);

	return 1;

}

int buscaNaNv2(csv_t *csv, char **v, unsigned long linha, int numCol) {

	for (int i = 0; i < numCol; i++) {
		if (strcmp(csv->matriz[linha][buscaPos(csv,v[i])],"NaN") == 0)
			return 1;
	}
	return 0;

}

int buscaNaN(csv_t *csv, int linha) {

	for (int j = 0; j < csv->numColunas; j++) 
		if (strcmp(csv->matriz[linha][j],"NaN") == 0)
			return 1;
	return 0;
}

void menuDadosFaltantes() {

	printf("\t1) Listas registros com NaN\n");
	printf("\t2) Substituir pela media\n");
	printf("\t3) Substituir pelo proximo valor valido\n");
	printf("\t4) Remover registros com NaN\n");
	printf("\t5) Voltar ao menu principal\n");
	printf("\tDigite sua opcao: ");

}

float calcMedia(csv_t *csv, char ***matriz, int col, int numLin) {

	float soma = 0; 
	unsigned long cont = 0;
	for (int i = 1; i < numLin; i++) {
		if (strcmp(matriz[i][col], "NaN") != 0) {
			cont++;
			soma += atof(matriz[i][col]);
		}
	}
	return soma/cont;

}

void troca(unsigned long *v, int a, int b) {
	unsigned long aux = v[a];
	v[a] = v[b];
	v[b] = aux;
}

void selectionSort(unsigned long *v, unsigned long contLin) {
	
	unsigned long menor, lim = 10;
	if (contLin < 10)
		lim = contLin - 1;
	for (int i = 5; i < lim; i++) {
		menor = i;
		for (int j = i+1; j < lim; j++)
			if (v[j] < v[menor])
				menor = j;
		troca(v,menor,i);
	}

}

int imprimeMatriz(csv_t *csv, char ***matriz, int minLin, int maxLin, int minCol, int maxCol, unsigned long *posicoes) {

	char indice[20], nLin[20];
	int cont;
	if (minLin == maxLin-5)
		cont = 5;
	else
		cont = 0;

	snprintf(nLin,sizeof(nLin),"%ld",csv->numLinhas); //Converte um int em string

	for (int i = minLin; i < maxLin; i++) {
		if (i == 0) {
			if ((imprimeFormatado(" ", strlen(nLin),1)) == 0)
				return 0;
		}
		else {
			if ((posicoes != NULL) && (cont < csv->numLinhas-1)){
				snprintf(indice,sizeof(indice),"%ld",posicoes[cont]);
				if ((imprimeFormatado(indice,strlen(nLin),1)) == 0)
					return 0;
				cont++;
			}
			else if (posicoes == NULL) {
				snprintf(indice,sizeof(indice),"%d",i-1); 
				if ((imprimeFormatado(indice,strlen(nLin),1)) == 0)
					return 0;           //Imprime uma string formatada
			}
		}
		for (int j = minCol; j < maxCol; j++)
			if ((imprimeFormatado(matriz[i][j],csv->sizes[buscaPos(csv,matriz[0][j])],0)) == 0)
				return 0;
		printf("\n");

	}

	if ((maxLin == 6) && (maxLin < csv->numLinhas)) {
		if ((imprimeFormatado("...",strlen(nLin),1)) == 0)
			return 0;
		for (int j = minCol; j < maxCol; j++)
			if ((imprimeFormatado("...",csv->sizes[buscaPos(csv,matriz[0][j])],0)) == 0)
				return 0;
		printf("\n");
	}

	return 1;

}

int imprimeFormatado(char *string, int tamMaior, int indice) {

	int tamMenor = strlen(string);
	char *formatado;

	//printf("String: %s, TamMenor: %d, tamMaior: %d\n", string, tamMenor, tamMaior);
	//Imprime apenas o 0, pois se a string possuir somente um 0 da erro
	if (strcmp(string,"0") == 0) {
		printf("0");
		for (int i = 0; i < tamMaior-1; i++)
			printf(" ");
		putchar(' ');
		return 1;
	}

	//Cria um vetor com o tamanho da maior palavra na coluna
	if ((formatado = (char*) calloc(tamMaior,sizeof(char))) == 0)
		return 0;

	if (indice == 0) {
		//Guarda nesse vetor a string atual e espacos, caso necessarios
		for (int i = tamMaior - 1; i >= 0; i--) {// _string string 
			if (tamMenor <= 0)
				formatado[i] = ' ';
			else {
				formatado[i] = string[tamMenor-1];
				tamMenor--;
			}

		}
	}
	else if (indice == 1) {
		for (int i = 0; i < tamMaior; i++) { //_ 1
			if (i >= tamMenor)
				formatado[i] = ' ';
			else {
				formatado[i] = string[i];
			}
		}
	}

	//Imprime o vetor
	for (int i = 0; i < tamMaior; i++)
		printf("%c", formatado[i]);
	putchar(' ');

	//Libera a memoria do vetor
	free(formatado);

	return 1;
}

int descartaOriginal(csv_t *csv, int numLin, int numCol) {

	char op;
	
	op = getOp(0);
	getchar();

	if (op == 'S')//Se op == S cria um novo arquivo com os dados da matrizAux
		salvaDados(csv,csv->matrizAux,numLin,numCol); 


	op = getOp(1);
	if (op == 'S') { //Se op == S da free na matriz original, cria uma nova matriz original com os dados de matrizAux, e da free em matrizAux
		liberaMatriz(csv,csv->matriz,csv->numLinhas,csv->numColunas);
		if ((csv->matriz = (char***) malloc(numLin * sizeof(char**))) == NULL)
			return 0;
		for (int i = 0; i < numLin; i++) {
			if ((csv->matriz[i] = (char**) malloc(numCol * sizeof(char*))) == NULL)
				return 0;
			for (int j = 0; j < numCol; j++)
				csv->matriz[i][j] = strdup(csv->matrizAux[i][j]);
		}
		liberaMatriz(csv,csv->matrizAux,numLin,numCol);
		csv->numLinhas = numLin;
		csv->numColunas = numCol;
	}
	else if (op == 'N')//Se op == N, da free na matrizAux
		liberaMatriz(csv,csv->matrizAux,numLin,numCol);

	return 1;

}

int compara(char *word, char *valor, char *filtro, int tipo) {
	
	if (strcmp(filtro,"==") == 0)
		return comparaIgual(word,valor,tipo);

	if (strcmp(filtro,">") == 0)
		return comparaMaior(word,valor,tipo);

	if (strcmp(filtro,">=") == 0)
		return comparaMaiorIgual(word,valor,tipo);

	if (strcmp(filtro,"<") == 0)
		return comparaMenor(word,valor,tipo);

	if (strcmp(filtro,"<=") == 0)
		return comparaMenorIgual(word,valor,tipo);

	if (strcmp(filtro,"!=") == 0)
		return comparaDiff(word,valor,tipo);

	return 0;
}

int comparaIgual(char *word, char *valor, int tipo) {
	
	if (tipo == 1) {
		int num1 = atof(word);
		int num2 = atof(valor);
		if (num1 == num2)
			return 1;
		return 0;
	}

	else if (strcmp(word,valor) == 0)
		return 1;
	return 0;

}

int comparaMaior(char *word, char *valor, int tipo) {
	
	if (tipo == 1) {
		int num1 = atof(word);
		int num2 = atof(valor);
		if (num1 > num2)
			return 1;
		return 0;
	}

	if (strcmp(word,valor) > 0)
		return 1;
	return 0;

}

int comparaMaiorIgual(char *word, char *valor, int tipo) {
	
	if (tipo == 1) {
		int num1 = atof(word);
		int num2 = atof(valor);
		if (num1 >= num2)
			return 1;
		return 0;
	}

	if ((strcmp(word,valor) == 0) || (strcmp(word,valor) > 0))
		return 1;
	return 0;

}

int comparaMenor(char *word, char *valor, int tipo) {
	
	if (tipo == 1) {
		int num1 = atof(word);
		int num2 = atof(valor);
		if (num1 < num2)
			return 1;
		return 0;
	}

	if (strcmp(word,valor) < 0)
		return 1;
	return 0;

}

int comparaMenorIgual(char *word, char *valor, int tipo){
	if (tipo == 1) {
		int num1 = atof(word);
		int num2 = atof(valor);
		if (num1 <= num2)
			return 1;
		return 0;
	}

	if ((strcmp(word,valor) == 0) || (strcmp(word,valor) < 0))
		return 1;
	return 0;

}

int comparaDiff(char *word, char *valor, int tipo){
	if (tipo == 1) {
		int num1 = atof(word);
		int num2 = atof(valor);
		if (num1 != num2)
			return 1;
		return 0;
	}

	if (strcmp(word,valor) != 0)
		return 1;
	return 0;

}

int buscaPos(csv_t *csv, char *var) {

	for (int j = 0; j < csv->numColunas; j++)
		if (strcmp(csv->matriz[0][j],var) == 0)
			return j;
	return -1;
}

int compString( const void *str1, const void *str2 ) {

	return strcmp((((var_t*)str1)->string), (((var_t*)str2)->string));

}

int compFloat(const void* elem1, const void* elem2) {

	if (strcmp(((var_t*)elem1)->string,"NaN") == 0)
		return -1;
	else if (strcmp(((var_t*)elem2)->string,"NaN") == 0)
		return 1;

	if (atof(((var_t*)elem1)->string) < atof(((var_t*)elem2)->string))
		return -1;
	return  atof(((var_t*)elem1)->string) > atof(((var_t*)elem2)->string); 

}

void sort(var_t *vetor, int count, int tipo) {

	if (tipo == 0)
		qsort(vetor, count, sizeof(var_t), compString);
	else
		qsort(vetor, count, sizeof(var_t), compFloat);

}
//Da close no arquivo e da todos os frees necessarios
void liberaCsv(csv_t *csv) {
	
	fclose(csv->arquivo);
	liberaMatriz(csv,csv->matriz, csv->numLinhas, csv->numColunas);
	free(csv->sizes);
	free(csv->tipo);

}

void liberaMatriz(csv_t *csv, char ***matriz, int numLin, int numCol) {

	for(int i = 0; i < numLin; i++) {
		for (int j = 0; j < numCol; j++)
			free(matriz[i][j]);
		free(matriz[i]);
	}
	free(matriz); 
}
