#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO_ENTRADA "code.bin"
#define TAMANHO_RAM 256
#define TAMANHO_PALAVRA 4

int PC;
int IR;
int SaidaUAL;
int RegistradorA;
int RegistradorB;
int MDR;
int BCO_REG[32];
char estadoAtual;
char estadoFuturo;
unsigned int bitsDeControle;
unsigned char RAM[TAMANHO_RAM];

void InicializaVariaveisGlobais(){
	IR=0;
	PC=0;
	SaidaUAL=0;
	estadoAtual=0;
	RegistradorA=0;
	RegistradorB=0;
	MDR=0;
	estadoFuturo=0;
	bitsDeControle=0;
	for(int i=0; i<TAMANHO_RAM; i++)
		RAM[i]=0;
	for(int i=0; i<32; i++)
		BCO_REG[i]=0;
}


void UnidadeDeControle(int codOp){
	char EA3, EA2, EA1, EA0;
	EA0 = estadoAtual % 2;
	EA1 = (estadoAtual >> 1) % 2;
	EA2 = (estadoAtual >> 2) % 2;
	EA3 = (estadoAtual >> 3) % 2;

	char OP5, OP4, OP3, OP2, OP1, OP0;
	OP0 = codOp % 2;
	OP1 = (codOp >> 1) % 2;
	OP2 = (codOp >> 2) % 2;
	OP3 = (codOp >> 3) % 2;
	OP4 = (codOp >> 4) % 2;
	OP5 = (codOp >> 5) % 2;

	char EF3, EF2, EF1, EF0;
	char MemParaReg1, MemParaReg0;
	char IREsc, BNE, EscMem, LerMem;
	char IouD, PCEsc, PcEscCond;
	char FontePC1, FontePC0;
	char UALOp1, UALOp0;
	char UALFonteB1, UALFonteB0, UALFonteA;
	char EscReg, RegDst1, RegDst0;

	EF3 = (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & ~OP3 & OP2 & ~OP1 & ~OP0) | (~EA3 & 
	~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & ~OP3 & ~OP2 & OP1 & ~OP0) | (~EA3 & ~EA2 & ~EA1 & 
	EA0 & ~OP5 & ~OP4 & ~OP3 & ~OP2 & OP1 & OP0) | (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & 
	OP4 & ~OP3 & OP2 & ~OP1 & OP0) |(~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & OP4 & ~OP3 & OP2
	& ~OP1 & ~OP0) | (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & OP3 & OP2 & ~OP1 & ~OP0) 
	| (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & ~OP3 & OP2 & ~OP1 & OP0) | (~EA3 & ~EA2 
	& EA1 & ~EA0 & ~OP5 & ~OP4 & OP3 & ~OP2 & ~OP1 & ~OP0) | (EA3 & EA2 & ~EA1 & EA0);

	EF2 = (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & ~OP3 & ~OP2 & ~OP1 & ~OP0) | (~EA3 & 
	~EA2 & ~EA1 & EA0 & ~OP5 & OP4 & ~OP3 & OP2 & ~OP1 & ~OP0) | (~EA3 & ~EA2 & ~EA1 & 
	EA0 & ~OP5 & ~OP4 & OP3 & OP2 & ~OP1 & ~OP0) | (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & 
	~OP4 & ~OP3 & OP2 & ~OP1 & OP0) | (~EA3 & ~EA2 & EA1 & ~EA0 & OP5 & ~OP4 & OP3 & 
	~OP2 & OP1 & OP0) | (~EA3 & ~EA2 & EA1 & ~EA0 & ~OP5 & ~OP4 & OP3 & ~OP2 & ~OP1 & 
	~OP0) | (~EA3 & ~EA2 & EA1 & EA0) | (~EA3 & EA2 & EA1 & ~EA0) | (EA3 & EA2 & ~EA1 & 
	EA0);

	EF1 = (~EA3 & ~EA2 & ~EA1 & EA0 & OP5 & ~OP4 & ~OP3 & ~OP2 & OP1 & OP0) | (~EA3 & ~EA2 
	& ~EA1 & EA0 & OP5 & ~OP4 & OP3 & ~OP2 & OP1 & OP0) | (~EA3 & ~EA2 & ~EA1 & EA0 & 
	~OP5 & ~OP4 & OP3 & ~OP2 & ~OP1 & ~OP0) | (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & 
	~OP3 & ~OP2 & ~OP1 & ~OP0) |(~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & ~OP3 & ~OP2 & 
	OP1 & OP0) | (~EA3 & ~EA2 & ~EA1 & EA0 &~OP5 & OP4 & ~OP3 & OP2 & ~OP1 & OP0) | (~EA3 
	& ~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & ~OP3 & OP2 & ~OP1 & OP0) | (~EA3 & ~EA2 & EA1 &
	~EA0 & OP5 & ~OP4 & ~OP3 & ~OP2 & OP1 & OP0) | (~EA3 & ~EA2 & EA1 & ~EA0 & ~OP5 & 
	~OP4 & OP3 & ~OP2 & ~OP1 & ~OP0) | (~EA3 & EA2 & EA1 & ~EA0);

	EF0 = (~EA3 & ~EA2 & ~EA1 & ~EA0) | (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & ~OP3 & 
	~OP2 & OP1& ~OP0) | (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & OP4 & ~OP3 & OP2 & ~OP1 & OP0)
	| (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & OP3 & OP2 & ~OP1 & ~OP0) | (~EA3 & ~EA2
	& ~EA1 & EA0 & ~OP5 & ~OP4 &~OP3 & OP2 & ~OP1 & OP0) | (~EA3 & ~EA2 & EA1 & ~EA0 & 
	OP5 & ~OP4 & ~OP3 & ~OP2 & OP1 & OP0) | (~EA3 & ~EA2 & EA1 & ~EA0 & OP5 & ~OP4 & OP3
	& ~OP2 & OP1 & OP0) | (~EA3 & EA2 & EA1 & ~EA0) | (EA3 & EA2 & ~EA1 & EA0);

	MemParaReg1 = (EA3 & ~EA2 & EA1 & ~EA0) | (EA3 & ~EA2 & EA1 & EA0);

	MemParaReg0 = (~EA3 & EA2 & ~EA1 & ~EA0);

	IREsc = (~EA3 & ~EA2 & ~EA1 & ~EA0);

	BNE = (~EA3 & ~EA2 & ~EA1 & ~EA0);

	EscMem = (~EA3 & EA2 & ~EA1 & EA0);

	LerMem = (~EA3 & ~EA2 & ~EA1 & ~EA0) | (~EA3 & ~EA2 & EA1 & EA0);

	IouD = (~EA3 & ~EA2 & EA1 & EA0) | (~EA3 & EA2 & ~EA1 & EA0);

	PCEsc = (~EA3 & ~EA2 & ~EA1 & ~EA0) | (EA3 & ~EA2 & ~EA1 & EA0) | (EA3 & ~EA2 & EA1 &
	~EA0) | (EA3 & ~EA2 & EA1 & EA0) | (EA3 & EA2 & ~EA1 & ~EA0);

	PcEscCond = (EA3 & ~EA2 & ~EA1 & ~EA0) | (EA3 & EA2 & EA1 & EA0);

	FontePC1 = (EA3 & ~EA2 & EA1 & ~EA0) | (EA3 & ~EA2 & EA1 & EA0) | (EA3 & EA2 & ~EA1 &
	~EA0);

	FontePC0 = (EA3 & ~EA2 & ~EA1 & ~EA0) | (EA3 & ~EA2 & EA1 & EA0) | (EA3 & EA2 & ~EA1 &
	~EA0) | (EA3 & EA2 & EA1 & EA0);

	UALOp1 = (~EA3 & EA2 & EA1 & ~EA0) | (EA3 & EA2 & ~EA1 & EA0);

	UALOp0 = (EA3 & ~EA2 & ~EA1 & ~EA0) | (EA3 & EA2 & ~EA1 & EA0) | (EA3 & EA2 & EA1 & EA0);

	UALFonteB1 = ~((~EA3 & ~EA2 & ~EA1 & ~EA0) | (~EA3 & EA2 & EA1 & ~EA0) | (EA3 & ~EA2 &
	~EA1 & ~EA0) | (EA3 & EA2 & EA1 & EA0));

	UALFonteB0 = (~EA3 & ~EA2 & ~EA1 & ~EA0) | (~EA3 & ~EA2 & ~EA1 & EA0 & OP5 & ~OP4 & 
	~OP3 & ~OP2 & OP1 & OP0) | (~EA3 & ~EA2 & ~EA1 & EA0 & OP5 & ~OP4 & OP3 & ~OP2 & OP1
	& OP0) | (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & OP3 & ~OP2 & ~OP1 & ~OP0) | (~EA3
	& ~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & ~OP3 & ~OP2 & ~OP1 & ~OP0) | (~EA3 & ~EA2 & ~EA1
	& EA0 & ~OP5 & ~OP4 & ~OP3 & OP2 & ~OP1 & ~OP0) | (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 &
	~OP4 & ~OP3 & ~OP2 & OP1 & ~OP0) | (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & ~OP3 & 
	~OP2 & OP1 & OP0) | (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & OP4 & ~OP3 & OP2 & ~OP1 & OP0)
	| (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & OP4 & ~OP3 & OP2 & ~OP1 & ~OP0) | (~EA3 & ~EA2 
	& ~EA1 & EA0 & ~OP5 & ~OP4 & OP3 & OP2 & ~OP1 & ~OP0) | (~EA3 & ~EA2 & ~EA1 & EA0 & 
	~OP5 & ~OP4 & ~OP3 & OP2 & ~OP1 & OP0);

	UALFonteA = (~EA3 & ~EA2 & EA1 & ~EA0 & OP5 & ~OP4 & ~OP3 & ~OP2 & OP1 & OP0) | (~EA3 
	& ~EA2 & EA1 & ~EA0 & OP5 & ~OP4 & OP3 & ~OP2 & OP1 & OP0) | (~EA3 & ~EA2 & EA1 & 
	~EA0 & ~OP5 & ~OP4 & OP3 & ~OP2 & ~OP1 & ~OP0) | (~EA3 & EA2 & EA1 & ~EA0) | (EA3 & 
	~EA2 & ~EA1 & ~EA0) | (EA3 & EA2 & EA1 & EA0) | (EA3 & EA2 & ~EA1 & EA0);

	EscReg = (~EA3 & EA2 & ~EA1 & ~EA0) | (~EA3 & EA2 & EA1 & EA0) | (EA3 & ~EA2 & EA1 &
 	~EA0) | (EA3 & ~EA2 & EA1 & EA0) | (EA3 & EA2 & EA1 & ~EA0);

	RegDst1 = (EA3 & ~EA2 & EA1 & ~EA0);

	RegDst0 = (~EA3 & EA2 & EA1 & EA0);

	bitsDeControle = (RegDst0%2) + (RegDst1%2)*2 + (EscReg%2)*4 + (UALFonteA%2)*8 + 
	(UALFonteB0%2)*16 + (UALFonteB1%2)*32 + (UALOp0%2)*64 * (UALOp1%2)*128 + (FontePC0%2)*256 + 
	(FontePC1%2)*512 + (PcEscCond%2)*1024 + (PCEsc%2)*2048 + (IouD%2)*4096 + (LerMem%2)*8192 + 
	(EscMem%2)*16384 + (BNE%2)*32768 + (IREsc%2)*65536 + (MemParaReg0%2)*131072 +
	(MemParaReg1%2)*262144;

	estadoFuturo = (EF0%2) + (EF1%2)*2 + (EF2%2)*4 + (EF3%2)*8;	
} 

int muxFontePC(int UALResult){
	int seleciona;
	seleciona  = (bitsDeControle >> 8) % 2;
	seleciona += 2*((bitsDeControle >> 9) % 2);


	switch(seleciona){
		case 0:
			return UALResult;
		break;
		case 1:
			return SaidaUAL;
		break;
		case 2:
			return //IRresult() << 2; //função que o oliver vai fazer que pega o conteudo da instrução
		break;
		case 3:
			return RegistradorA;
		break;
	}
}

int muxUALFonteB(){
	int seleciona;
	seleciona  = (bitsDeControle >> 4) % 2;
	seleciona += 2*((bitsDeControle >> 5) % 2);


	switch(seleciona){
		case 0:
			return RegistradorB;
		break;
		case 1:
			return 4;
		break;
		case 2:
			return //IRresult(); //função que o oliver vai fazer que pega o conteudo da instrução
		break;
		case 3:
			return //IRresult() << 2;
		break;
	}
}
int muxUALFonteA(){
	int seleciona;
	seleciona  = (bitsDeControle >> 3) % 2;

	switch(seleciona){
		case 0:
			return PC;
		break;
		case 1:
			return RegistradorA;
		break;
	}
}
int muxBNE(int UALZero){
	int seleciona;
	seleciona  = (bitsDeControle >> 15) % 2;

	switch(seleciona){
		case 0:
			return UALZero;
		break;
		case 1:
			return ~UALZero;
		break;
	}
}
int muxIouD(){
	int seleciona;
	seleciona  = (bitsDeControle >> 12) % 2;

	switch(seleciona){
		case 0:
			return PC;
		break;
		case 1:
			return SaidaUAL;
		break;
	}
}

int muxRegDest(int reg){	
	int RegDest; //pegar dos bitsDeControle o sinal que determina qual sera passado para frente
	RegDest = bitsDeControle % 2;
	RegDest = RegDest + (2*(bitsDeControle >> 1) % 2);

	switch(RegDest){
		case 0:
			return reg; 
		break;

		case 1:
			return reg;
		break;

		case 2:
			return 31;
		break;
	}
}

int muxMemParaReg(){
	int MemParaReg;

	MemParaReg = (bitsDeControle >> 17) % 2;
	MemParaReg = MemParaReg + (2*(bitsDeControle >> 18) % 2);

	switch(MemParaReg){
		case 0:
			return SaidaUAL; 
		break;

		case 1:
			return MDR; //aqui ainda nao se sabe de nada
		break;

		case 2:
			return PC;
		break;
	}

}

int UALcontrole(){
	int op, instrucao;
	op = (bitsDeControle >> 6) % 2;
	op+= 2*((bitsDeControle >> 7) % 2);
	//instrucao= //IRresult() função do Oliver;


	switch(op){
		case 0:
			return 0;//representa soma para a ULA
		break;
		case 1:
			return 1;//representa subtração
		break;
		case 2://de acordo com a instrução
			if(instrucao==32)
				return 0;//add 
			else if(instrucao==34)
				return 1;//sub
			else if(instrucao==36)
				return 2;//and
			else if(instrucao==37)
				return 3;//or
			else if(instrucao==42)
				return 4;//slt
		break;
		case 3:
			return 2;//and
		break;
	}
}

void UAL(){

}

void BR(){

}

void Memoria(){

}

void Instrucao(){

}

void MDR(){

}


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

int main(int argc, char const *argv[]){

	int instrucoes = LeInstrucoesDaEntrada(ARQUIVO_ENTRADA);
	if (instrucoes <= 0) {
		printf("ERRO! Não foi possível abrir o arquivo de entrada.\n");
		return 1;
	}


	return 0;
}
