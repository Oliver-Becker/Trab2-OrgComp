#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO_ENTRADA "code.bin"
#define TAMANHO_RAM 256
#define TAMANHO_PALAVRA 4

unsigned char RAM[TAMANHO_RAM];

int LeInstrucoesDaEntrada(char *arquivoEntrada) {
	int byteOffset = 0;
	if (arquivoEntrada == NULL)
		return byteOffset;

	FILE *fp = fopen(arquivoEntrada, "rb");
	if (fp == NULL)
		return byteOffset;

	fread(RAM + byteOffset, sizeof(char), 4, fp);

	while (!feof(fp)) {
		byteOffset += TAMANHO_PALAVRA;
		if (byteOffset >= TAMANHO_RAM - TAMANHO_PALAVRA)
			return -1;
		fread(RAM + byteOffset, sizeof(char), 4, fp);
	}

	return 1;
}

int main() {

	int instrucoes = LeInstrucoesDaEntrada(ARQUIVO_ENTRADA);
	if (instrucoes <= 0) {
		printf("ERRO! Não foi possível abrir o arquivo de entrada.\n");
		return 1;
	}


	return 0;
}
