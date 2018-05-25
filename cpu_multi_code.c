#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO_ENTRADA "code.bin"

int *LeInstrucoesDaEntrada(char *arquivoEntrada) {
	if (arquivoEntrada == NULL)
		return NULL;

	FILE *fp = fopen(arquivoEntrada, "rb");
	if (fp == NULL)
		return NULL;

	int tamanho = 0;
	int *instrucoes = NULL;
	int aux;
	fread(&aux, sizeof(int), 1, fp);

	while (!feof(fp)) {
		instrucoes = realloc(instrucoes, sizeof(instrucoes) * (++tamanho + 1));
		instrucoes[0] = tamanho;
		instrucoes[tamanho] = aux;
		fread(&aux, sizeof(int), 1, fp);
	}

	return instrucoes;
}

int main() {

	int *instrucoes = LeInstrucoesDaEntrada(ARQUIVO_ENTRADA);
	if (instrucoes == NULL) {
		printf("ERRO! Não foi possível abrir o arquivo de entrada.\n");
		return 1;
	}


	return 0;
}
