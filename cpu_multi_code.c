#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO_ENTRADA "code.bin"
#define TAMANHO_RAM 1024
#define TAMANHO_PALAVRA 4

// Operações da ULA
#define ADD 0
#define SUB 1
#define AND 2
#define OR  3
#define SLT 4

typedef unsigned int registrador;

typedef struct {
	unsigned funct:6;
	unsigned shamt:5;
	unsigned rd:5;
	unsigned rt:5;
	unsigned rs:5;
	unsigned op:6;
} TIPO_R;

typedef struct {
	unsigned immediate:16;
	unsigned rt:5;
	unsigned rs:5;
	unsigned op:6;
} TIPO_I;

typedef struct {
	unsigned address:26;
	unsigned op:6;
} TIPO_J;

typedef union {
	unsigned instrucao;
	TIPO_R r;
	TIPO_I i;
	TIPO_J j;
} INSTRUCAO;

typedef union {
	unsigned int inteiro;
	unsigned char byte[4];
} MEMORIA;

typedef struct {
	unsigned RegDst:2;
	unsigned EscReg:1;
	unsigned UALFonteA:1;
	unsigned UALFonteB:2;
	unsigned UALOp:2;
	unsigned FontePC:2;
	unsigned PCEscCond:1;
	unsigned PCEsc:1;
	unsigned IouD:1;
	unsigned LerMem:1;
	unsigned EscMem:1;
	unsigned BNE:1;
	unsigned IREsc:1;
	unsigned MemParaReg:2;
} BITS_CONTROLE;

typedef union {
	unsigned sinal;
	BITS_CONTROLE bits;
} SINAL_CONTROLE;

unsigned int PC;
INSTRUCAO IR;
unsigned int MDR;
unsigned int SaidaUAL;
unsigned int RegistradorA;
unsigned int RegistradorB;
int BCO_REG[32];
char estadoAtual;
char estadoFuturo;
SINAL_CONTROLE sinalDeControle;
unsigned char RAM[TAMANHO_RAM];

void InicializaVariaveisGlobais(){
	IR.instrucao = 0;
	PC = 0;
	SaidaUAL = 0;
	estadoAtual = 0;
	RegistradorA = 0;
	RegistradorB = 0;
	MDR = 0;
	estadoFuturo = 0;
	sinalDeControle.sinal = 0;

	for(int i = 0; i < TAMANHO_RAM; i++)
		RAM[i] = 0;
	for(int i = 0; i < 32; i++)
		BCO_REG[i] = 0;
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
	unsigned char FontePC1, FontePC0;
	unsigned char UALOp1, UALOp0;
	unsigned char UALFonteB1, UALFonteB0;
	unsigned char RegDst1, RegDst0;

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

	sinalDeControle.bits.IREsc = (~EA3 & ~EA2 & ~EA1 & ~EA0);

	sinalDeControle.bits.BNE = (~EA3 & ~EA2 & ~EA1 & ~EA0);

	sinalDeControle.bits.EscMem = (~EA3 & EA2 & ~EA1 & EA0);

	sinalDeControle.bits.LerMem = (~EA3 & ~EA2 & ~EA1 & ~EA0) | (~EA3 & ~EA2 & EA1 & EA0);

	sinalDeControle.bits.IouD = (~EA3 & ~EA2 & EA1 & EA0) | (~EA3 & EA2 & ~EA1 & EA0);

	sinalDeControle.bits.PCEsc = (~EA3 & ~EA2 & ~EA1 & ~EA0) | (EA3 & ~EA2 & ~EA1 & EA0) |
	(EA3 & ~EA2 & EA1 & ~EA0) | (EA3 & ~EA2 & EA1 & EA0) | (EA3 & EA2 & ~EA1 & ~EA0);

	sinalDeControle.bits.PCEscCond = (EA3 & ~EA2 & ~EA1 & ~EA0) | (EA3 & EA2 & EA1 & EA0);

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

	sinalDeControle.bits.UALFonteA = (~EA3 & ~EA2 & EA1 & ~EA0 & OP5 & ~OP4 & ~OP3 & ~OP2 & OP1 
	& OP0) | (~EA3 & ~EA2 & EA1 & ~EA0 & OP5 & ~OP4 & OP3 & ~OP2 & OP1 & OP0) | (~EA3 & ~EA2 & 
	EA1 & ~EA0 & ~OP5 & ~OP4 & OP3 & ~OP2 & ~OP1 & ~OP0) | (~EA3 & EA2 & EA1 & ~EA0) | (EA3 & 
	~EA2 & ~EA1 & ~EA0) | (EA3 & EA2 & EA1 & EA0) | (EA3 & EA2 & ~EA1 & EA0);

	sinalDeControle.bits.EscReg = (~EA3 & EA2 & ~EA1 & ~EA0) | (~EA3 & EA2 & EA1 & EA0) |
	(EA3 & ~EA2 & EA1 & ~EA0) | (EA3 & ~EA2 & EA1 & EA0) | (EA3 & EA2 & EA1 & ~EA0);

	RegDst1 = (EA3 & ~EA2 & EA1 & ~EA0);

	RegDst0 = (~EA3 & EA2 & EA1 & EA0);

	sinalDeControle.bits.RegDst = (RegDst1 << 2) + RegDst0;
	sinalDeControle.bits.UALFonteB = (UALFonteB1 << 2) + UALFonteB0;
	sinalDeControle.bits.UALOp = (UALOp1 << 2) + UALOp0;
	sinalDeControle.bits.FontePC = (FontePC1 << 2) + FontePC0;
	sinalDeControle.bits.MemParaReg = (MemParaReg1 << 2) + MemParaReg0;

	estadoFuturo = (EF0%2) + (EF1%2)*2 + (EF2%2)*4 + (EF3%2)*8;
	//printf("UC = %d EF0 = %d EF1 = %d EF2 = %d EF3 = %d \n", estadoFuturo, EF0, EF1, EF2, EF3 );
	//printf("Estado Futuro= %d\n", estadoFuturo);
} 

int ExtensaoDeSinal(short int valor16Bits) {
	return (int)valor16Bits;
}

int Mux1Bit(int sinal, int valorSe0, int valorSe1) {
	switch(sinal) {
		case 0:
			return valorSe0;
		case 1:
			return valorSe1;
	}
	return -1;
}

int Mux2Bit(int sinal, int valorSe0, int valorSe1, int valorSe2, int valorSe3) {
	switch(sinal) {
		case 0:
			return valorSe0;
		case 1:
			return valorSe1;
		case 2:
			return valorSe2;
		case 3:
			return valorSe3;
	}
	return -1;
}

int MuxFontePC(int UALResult) {
	return Mux2Bit(sinalDeControle.bits.FontePC, UALResult, SaidaUAL, IR.j.address, 
			RegistradorA);
}

int MuxUALFonteB() {
	return Mux2Bit(sinalDeControle.bits.UALFonteB, RegistradorB, 4,
			ExtensaoDeSinal(IR.i.immediate), ExtensaoDeSinal(IR.i.immediate) << 2);
}

int MuxUALFonteA() {
	return Mux1Bit(sinalDeControle.bits.UALFonteA, PC, RegistradorA);
}

int MuxBNE(int UALZero) {
	return Mux1Bit(sinalDeControle.bits.BNE, UALZero, !UALZero);
}

int MuxIouD() {
	return Mux1Bit(sinalDeControle.bits.IouD, PC, SaidaUAL);
}

int MuxRegDest() {	
	return Mux2Bit(sinalDeControle.bits.RegDst, IR.r.rt, IR.r.rd, 31, -1);
}

int MuxMemParaReg() {
	return Mux2Bit(sinalDeControle.bits.MemParaReg, SaidaUAL, MDR, PC, -1);
}

int SelecionaOperacao(int campoDeFuncao) {
	switch(campoDeFuncao) {
		case 32:
			return ADD;
		case 34:
			return SUB;
		case 36:
			return AND;
		case 37:
			return OR;
		case 42:
			return SLT;
		default:
			return -1;
	}
	return -1;
}

int UALcontrole() {
	return Mux2Bit(sinalDeControle.bits.UALOp, ADD, SUB, SelecionaOperacao(IR.r.funct), AND);
}

int PortaEPC(int UALZero) {
	return sinalDeControle.bits.PCEscCond && MuxBNE(UALZero);
}

int PortaOUPC(int UALZero) {
	return sinalDeControle.bits.PCEsc || PortaEPC(UALZero);
}

void EscreveNoPC(int UALResult, int UALZero) {
	if(PortaOUPC(UALZero) == 1)
		PC = MuxFontePC(UALResult);
}

void BancoDeRegistradores(int *RegATemp, int *RegBTemp) {
	*RegATemp = BCO_REG[IR.r.rs];
	*RegBTemp = BCO_REG[IR.r.rt];

	if(sinalDeControle.bits.EscReg == 1)
		BCO_REG[MuxRegDest()] = MuxMemParaReg();
}

int UAL(int op, int *UALZero) {
	unsigned int a = MuxUALFonteA();
	unsigned int b = MuxUALFonteB();
	*UALZero = (a - b == 0) ? 1 : 0;

	switch(op) {
		case 0:
			return a + b;
		case 1:
			return a - b;
		case 2:
			return a & b;
		case 3:
			return a | b;
		case 4:
			return (a - b < 0) ? 1 : 0;
	}
}

int LePalavraDaMemoria(unsigned int byteOffset) {
	if (byteOffset > TAMANHO_RAM - TAMANHO_PALAVRA)
		return -1;

	MEMORIA memoria;

	for (int i = TAMANHO_PALAVRA; i > 0 ; --i)
		 memoria.byte[i-1] = RAM[byteOffset++];

	return memoria.inteiro;
}

int EscrevePalavraNaMemoria(unsigned int byteOffset, unsigned int palavra) {
	if (byteOffset > TAMANHO_RAM - TAMANHO_PALAVRA)
		return -1;

	MEMORIA memoria;
	memoria.inteiro = palavra;

	for (int i = TAMANHO_PALAVRA; i > 0 ; --i)
		RAM[byteOffset++] = memoria.byte[i-1];

	return 0;
}

int Memoria() {
	if(sinalDeControle.bits.LerMem) {
		return LePalavraDaMemoria(MuxIouD());
	}
	else if(sinalDeControle.bits.EscMem) {
		return EscrevePalavraNaMemoria(MuxIouD(), RegistradorB);
	}

	return 0;
}

int LeInstrucoesDaEntrada(char *arquivoEntrada) {
	int byteOffset = 0;
	if (arquivoEntrada == NULL)
		return byteOffset;

	FILE *fp = fopen(arquivoEntrada, "rb");
	if (fp == NULL)
		return byteOffset;

	MEMORIA memoria;
	fscanf(fp, "%d", &memoria.inteiro);

	while (!feof(fp)) {
		if (byteOffset >= TAMANHO_RAM - TAMANHO_PALAVRA)
			return -1;

		for (int i = TAMANHO_PALAVRA; i > 0 ; --i)
			RAM[byteOffset++] = memoria.byte[i-1];

		fscanf(fp, "%d", &memoria.inteiro);
	}

	return byteOffset;
}

void LeituraIR(int instrucao) {
	if (sinalDeControle.bits.IREsc)
		IR.instrucao = instrucao;
}

int main(int argc, char const *argv[]) {

	InicializaVariaveisGlobais();
	int instrucoes = LeInstrucoesDaEntrada(ARQUIVO_ENTRADA);
	if (instrucoes <= 0) {
		printf("ERRO! Não foi possível abrir o arquivo de entrada.\n");
		return instrucoes;
	}


	MEMORIA memoria;
	int count = instrucoes/4;
	printf("RAM: \n");
	for (int i = 0; i < count+3; i++)
	{
		memoria.byte[3] = RAM[(i*4)];
		memoria.byte[2] = RAM[(i*4)+1];
		memoria.byte[1] = RAM[(i*4)+2];
		memoria.byte[0] = RAM[(i*4)+3];
		printf("%u\n", memoria.inteiro);
	}

	unsigned int IRaux, ULA0;
	unsigned int regA, regB;
	unsigned int ULAres, ULAop;
	while (count) {
		//printf("count =  %d  estadoAtual =  %d estadoFuturo = %d\n", count, estadoAtual, estadoFuturo);
		//printf("Estado atual = %d\n", estadoAtual);
		//printf("count = %d, t8 = %d\n", count, BCO_REG[24]);
		//printf("IR = %u\n", IR);
		//printf("Retorno Func= %d\n", IR.j.op);

		UnidadeDeControle(IR.j.op);
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
	printf("IR: %u\n", IR.instrucao);
	printf("MDR: %u\n", MDR);
	printf("A: %u\n", RegistradorA);
	printf("B: %u\n", RegistradorB);
	printf("AluOut: %u\n", SaidaUAL);

	for (int i = 0; i < 32; ++i) {
		printf("Registrador %d: %d\n", i, BCO_REG[i]);
	}
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
