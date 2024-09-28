#include "io.h"

int main(int argc, char **argv) {

	int op;
	csv_t *csv;

	if (argc < 2) {
		printf("Faltaram argumentos na execucao do programa\n");
		return 1;
	}
	
	if ((csv = abreCsv(argv[1])) == NULL)
		return 1;

	menu();
	scanf("%d", &op);
	printf("\n");

	while (op != 9) {
		switch(op) {
			case 1:
				sumario(csv);
				break;
			case 2:
				if ((mostrar(csv,csv->matriz,csv->numLinhas,csv->numColunas,NULL)) == 0) {
					printf("Ocorreu um erro\n");
					return 1;
				}
				break;
			case 3:
				if ((filtro(csv,compara)) == 0) {
					printf("Ocorreu um erro\n");
					return 1;
				}
				break;
			case 4:
				if ((descricao(csv)) == 0) {
					printf("Ocorreu um erro\n");
					return 1;
				}
				break;
			case 5:
				if ((ordenacao(csv)) == 0) {
					printf("Ocorreu um erro\n");
					return 1;
				}
				break;
			case 6:
				if ((selecao(csv)) == 0) {
					printf("Ocorreu um erro\n");
					return 1;
				}
				break;
			case 7:
				if ((dadosFaltantes(csv)) == 0) {
					printf("Ocorreu um erro\n");
					return 1;
				}
				break;
			case 8:
				if ((salvaDados(csv,csv->matriz,csv->numLinhas,csv->numColunas)) == 0) {
					printf("Ocorreu um erro\n");
					return 1;
				}
				break;
			default:
				printf("Escolha uma opcao valida\n");
				break;
		}

		printf("\nPressione ENTER para continuar\n");
		getchar();
		getchar();
		menu();
		scanf("%d", &op);	
		
	}

	liberaCsv(csv);
	free(csv);

	return 0;
}
