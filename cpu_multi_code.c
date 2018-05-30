#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO_ENTRADA "code.bin"
#define TAMANHO_RAM 1024
#define TAMANHO_PALAVRA 4

typedef unsigned int registrador;
typedef unsigned char byte;

unsigned int PC;
unsigned int IR;
unsigned int MDR;
unsigned int SaidaUAL;
unsigned int RegistradorA;
unsigned int RegistradorB;
int BCO_REG[32];
char estadoAtual;
char estadoFuturo;
unsigned int bitsDeControle;
unsigned char RAM[TAMANHO_RAM];

union mem{
	unsigned int inteiro;
	unsigned char byte[4];
};

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

unsigned int LeRegistradorInstrucao(unsigned int bitInicial, unsigned int bitFinal) {
	if (bitInicial <= bitFinal || bitInicial > 31 || bitFinal < 0)
		return -1;

	return (unsigned int)(IR << (unsigned int)31 - bitInicial) >> (unsigned int)(31 - (unsigned int)bitInicial + bitFinal);
}

void UnidadeDeControle(int codOp){
	unsigned char EA3, EA2, EA1, EA0;
	EA0 = estadoAtual % 2;
	EA1 = (estadoAtual >> 1) % 2;
	EA2 = (estadoAtual >> 2) % 2;
	EA3 = (estadoAtual >> 3) % 2;

	unsigned char OP5, OP4, OP3, OP2, OP1, OP0;
	OP0 = codOp % 2;
	OP1 = (codOp >> 1) % 2;
	OP2 = (codOp >> 2) % 2;
	OP3 = (codOp >> 3) % 2;
	OP4 = (codOp >> 4) % 2;
	OP5 = (codOp >> 5) % 2;

	unsigned char EF3, EF2, EF1, EF0;
	unsigned char MemParaReg1, MemParaReg0;
	unsigned char IREsc, BNE, EscMem, LerMem;
	unsigned char IouD, PCEsc, PcEscCond;
	unsigned char FontePC1, FontePC0;
	unsigned char UALOp1, UALOp0;
	unsigned char UALFonteB1, UALFonteB0, UALFonteA;
	unsigned char EscReg, RegDst1, RegDst0;

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
	~OP4 & OP3 & ~OP2 & ~OP1 & ~OP0) | (~EA3 & EA2 & EA1 & ~EA0) | (EA3 & EA2 & ~EA1 & EA0);

	EF0 = (~EA3 & ~EA2 & ~EA1 & ~EA0) | (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & ~OP3 & 
	~OP2 & OP1& ~OP0) | (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & OP4 & ~OP3 & OP2 & ~OP1 & OP0)
	| (~EA3 & ~EA2 & ~EA1 & EA0 & ~OP5 & ~OP4 & OP3 & OP2 & ~OP1 & ~OP0) | (~EA3 & ~EA2
	& ~EA1 & EA0 & ~OP5 & ~OP4 &~OP3 & OP2 & ~OP1 & OP0) | (~EA3 & ~EA2 & EA1 & ~EA0 & 
	OP5 & ~OP4 & ~OP3 & ~OP2 & OP1 & OP0) | (~EA3 & ~EA2 & EA1 & ~EA0 & OP5 & ~OP4 & OP3
	& ~OP2 & OP1 & OP0) | (~EA3 & EA2 & EA1 & ~EA0);

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
	//printf("UC = %d EF0 = %d EF1 = %d EF2 = %d EF3 = %d \n", estadoFuturo, EF0, EF1, EF2, EF3 );
	//printf("Estado Futuro= %d\n", estadoFuturo);
} 

int MuxFontePC(int UALResult){
	unsigned int seleciona;
	seleciona  = (bitsDeControle >> 8) % 2;
	seleciona += 2*((bitsDeControle >> 9) % 2);

	switch(seleciona){
		case 0:
			return UALResult;

		case 1:
			return SaidaUAL;
		
		case 2:
			return LeRegistradorInstrucao(25, 0)<< 2;
		
		case 3:
			return RegistradorA;
	}
}

int MuxUALFonteB(){
	unsigned int seleciona;
	seleciona  = (bitsDeControle >> 4) % 2;
	seleciona += 2*((bitsDeControle >> 5) % 2);

	switch(seleciona){
		case 0:
			return RegistradorB;
	
		case 1:
			return 4;
	
		case 2:
			return LeRegistradorInstrucao(15, 0);
	
		case 3:
			return LeRegistradorInstrucao(15, 0) << 2;
	}
}
int MuxUALFonteA(){
	unsigned int seleciona;
	seleciona  = (bitsDeControle >> 3) % 2;

	switch(seleciona){
		case 0:
			return PC;
	
		case 1:
			return RegistradorA;
	}
}
int MuxBNE(int UALZero){
	unsigned int seleciona;
	seleciona  = (bitsDeControle >> 15) % 2;

	switch(seleciona){
		case 0:
			return UALZero;
	
		case 1:
			return ~UALZero;
	}
}
int MuxIouD(){
	unsigned int seleciona;
	seleciona  = (bitsDeControle >> 12) % 2;

	switch(seleciona){
		case 0:
			return PC;

		case 1:
			return SaidaUAL;
	}
}
//tirei o parametro da função e coloquei a fução do oliver pra selecionar o destino
int MuxRegDest(){	
	unsigned int RegDest; //pegar dos bitsDeControle o sinal que determina qual sera passado para frente
	RegDest = bitsDeControle % 2;
	RegDest = RegDest + (2*(bitsDeControle >> 1) % 2);

	switch(RegDest){
		case 0:
			return LeRegistradorInstrucao(20, 16); 

		case 1:
			return LeRegistradorInstrucao(15, 11);

		case 2:
			return 31;
	}
}

int MuxMemParaReg(){
	unsigned int MemParaReg;
	MemParaReg = (bitsDeControle >> 17) % 2;
	MemParaReg = MemParaReg + (2*(bitsDeControle >> 18) % 2);

	switch(MemParaReg){
		case 0:
			return SaidaUAL; 

		case 1:
			return MDR; //aqui ainda nao se sabe de nada

		case 2:
			return PC;
	}

}

int UALcontrole(){
	int op, instrucao;
	op = (bitsDeControle >> 6) % 2;
	op+= 2*((bitsDeControle >> 7) % 2);
	instrucao= LeRegistradorInstrucao(5, 0);

	switch(op){
		case 0:
			return 0;//representa soma para a ULA

		case 1:
			return 1;//representa subtração

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

		case 3:
			return 2;//and
	}
}

int PortaEPC(int UALZero){
	unsigned int PcEscCond;
	PcEscCond=(bitsDeControle >> 10) % 2;
	return PcEscCond & MuxBNE(UALZero)%2;
}

int PortaOUPC(int UALZero){
	unsigned int PCEsc;
	PCEsc=(bitsDeControle >> 11) % 2;
	return PCEsc | (PortaEPC(UALZero)%2);
}

void EscreveNoPC(int UALResult, int UALZero){
	if(PortaOUPC(UALZero)==1)
		PC=MuxFontePC(UALResult);
}

void BancoDeRegistradores(int *RegATemp, int *RegBTemp){
	*RegATemp=BCO_REG[LeRegistradorInstrucao(25, 21)];
	*RegBTemp=BCO_REG[LeRegistradorInstrucao(20, 16)];

	unsigned int EscReg=(bitsDeControle >> 2) % 2;

	if(EscReg==1)
		BCO_REG[MuxRegDest()]=MuxMemParaReg();
}

int UAL(int op, int *UALZero){
	unsigned int a= MuxUALFonteA();
	unsigned int b= MuxUALFonteB();
	(a-b==0) ? (*UALZero=1) : (*UALZero=0);

	switch(op){
		case 0:
			return a + b;

		case 1:
			return a - b;

		case 2:
			return a & b;

		case 3:
			return a | b;

		case 4:
			return (a-b<0) ? 1 : 0;
	}
}

int Memoria(){
	int LerMem = (bitsDeControle >> 13) % 2;
	int EscMem = (bitsDeControle >> 14) % 2;
	int aux = 0;
	
	if(LerMem==1){
		aux = RAM[MuxIouD()] << 24;
		aux+= RAM[MuxIouD()+1] << 16;
		aux+= RAM[MuxIouD()+2] << 8;
		aux+= RAM[MuxIouD()+3];
	}
	else if(EscMem==1){
		RAM[MuxIouD()]= RegistradorB >> 24;
		aux= RegistradorB << 8;
		RAM[MuxIouD()+1]= aux >> 24;
		aux= RegistradorB << 16;
		RAM[MuxIouD()+2]= aux >> 24;
		aux= RegistradorB << 24;
		RAM[MuxIouD()+3]= aux >> 24;
	}
	return aux;
}

int LeInstrucoesDaEntrada(char *arquivoEntrada) {
	int byteOffset = 0;
	if (arquivoEntrada == NULL)
		return byteOffset;

	FILE *fp = fopen(arquivoEntrada, "rb");
	if (fp == NULL)
		return byteOffset;
	int aux;
	//fread(&aux, sizeof(int), 1, fp);
	fscanf(fp, "%d", &aux);

	RAM[0]= aux >> 24;
	int aux2= aux << 8;
	RAM[1]= aux2 >> 24;
	aux2= aux << 16;
	RAM[2]= aux2 >> 24;
	aux2= aux << 24;
	RAM[3]= aux2 >> 24;

	while (!feof(fp)) {
		byteOffset += TAMANHO_PALAVRA;
		if (byteOffset >= TAMANHO_RAM - TAMANHO_PALAVRA)
			return -1;
		//fread(&aux, sizeof(int), 1, fp);
		fscanf(fp, "%d", &aux);
		RAM[byteOffset]= aux >> 24;
		aux2= aux << 8;
		RAM[byteOffset+1]= aux2 >> 24;
		aux2= aux << 16;
		RAM[byteOffset+2]= aux2 >> 24;
		aux2= aux << 24;
		RAM[byteOffset+3]= aux2 >> 24;
	}
	printf("ftell =%ld", ftell(fp));
	return byteOffset;

}

void LeituraIR(int instrucao) {
	if ((bitsDeControle >> 16) % 2)
		IR = instrucao;
}

int main(int argc, char const *argv[]){

	InicializaVariaveisGlobais();
	int instrucoes = LeInstrucoesDaEntrada(ARQUIVO_ENTRADA);
	if (instrucoes <= 0) {
		printf("ERRO! Não foi possível abrir o arquivo de entrada.\n");
		return instrucoes;
	}


	int count = instrucoes/4;
	printf("count: %d\n", count);
	for(int i = 0; i < count*4; ++i) {
		printf("%d", RAM[i]);
	}
	printf("\n");

	unsigned int IRaux, ULA0;
	unsigned int regA, regB;
	unsigned int ULAres, ULAop;
	while (count) {
		//printf("count =  %d  estadoAtual =  %d estadoFuturo = %d\n", count, estadoAtual, estadoFuturo);
		printf("Estado atual = %d\n", estadoAtual);
		printf("count = %d\n", count);
		printf("IR = %d\n", IR);
		//printf("Retorno Func= %d\n", LeRegistradorInstrucao(31, 26));

		UnidadeDeControle(LeRegistradorInstrucao(31, 26));
		IRaux = Memoria();
		BancoDeRegistradores(&regA, &regB);
		ULAop = UALcontrole();
		ULAres = UAL(ULAop, &ULA0);
		
		LeituraIR(IRaux);
		MDR = IRaux;
		RegistradorA = regA;
		RegistradorB = regB;
		SaidaUAL = ULAres;
		EscreveNoPC(ULAres, ULA0);
		estadoAtual = estadoFuturo;
		if(estadoAtual==0)
			count-=1;
	}

	printf("PC: %u\n", PC);
	printf("IR: %u\n", IR);
	printf("MDR: %u\n", MDR);
	printf("A: %u\n", RegistradorA);
	printf("B: %u\n", RegistradorB);
	printf("AluOut: %u\n", SaidaUAL);

	for (int i = 0; i < 32; ++i) {
		printf("Registrador %d: %d\n", i, BCO_REG[i]);
	}
	union mem memoria;
	printf("RAM: \n");
	for (int i = 0; i < 32; i++)
	{
		memoria.byte[3] = RAM[(i*4)];
		memoria.byte[2] = RAM[(i*4)+1];
		memoria.byte[1] = RAM[(i*4)+2];
		memoria.byte[0] = RAM[(i*4)+3];
		printf("%u ", memoria.inteiro);
	}
	printf("\n");
	return 0;
}
