#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_RAM 256
#define TAMANHO_PALAVRA 4

// Operações da ULA
#define ADD 0
#define SUB 1
#define AND 2
#define OR  3
#define SLT 4

enum {
	PROGNAME,
	ARQUIVO_ENTRADA,
	NARGS,
};

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

	sinalDeControle.bits.BNE = (EA3 & EA2 & EA1 & EA0);

	sinalDeControle.bits.EscMem = (~EA3 & EA2 & ~EA1 & EA0);

	sinalDeControle.bits.LerMem = (~EA3 & ~EA2 & ~EA1 & ~EA0) | (~EA3 & ~EA2 & EA1 & EA0);

	sinalDeControle.bits.IouD = (~EA3 & ~EA2 & EA1 & EA0) | (~EA3 & EA2 & ~EA1 & EA0);

	sinalDeControle.bits.PCEsc = (~EA3 & ~EA2 & ~EA1 & ~EA0) | (EA3 & ~EA2 & ~EA1 & EA0) |
	(EA3 & ~EA2 & EA1 & ~EA0) | (EA3 & ~EA2 & EA1 & EA0) | (EA3 & EA2 & ~EA1 & ~EA0);

	sinalDeControle.bits.PCEscCond = (EA3 & ~EA2 & ~EA1 & ~EA0) | (EA3 & EA2 & EA1 & EA0);

	FontePC1 = (EA3 & ~EA2 & EA1 & ~EA0) | (EA3 & ~EA2 & EA1 & EA0) | (EA3 & EA2 & ~EA1 &
	~EA0) | (EA3 & ~EA2 & ~EA1 & EA0);

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

	sinalDeControle.bits.RegDst = (RegDst1 << 1) + RegDst0 % 2;
	sinalDeControle.bits.UALFonteB = (UALFonteB1 << 1) + UALFonteB0 % 2;
	sinalDeControle.bits.UALOp = (UALOp1 << 1) + UALOp0 % 2;
	sinalDeControle.bits.FontePC = (FontePC1 << 1) + FontePC0 % 2;
	sinalDeControle.bits.MemParaReg = (MemParaReg1 << 1) + MemParaReg0 % 2;

	estadoFuturo = (EF0%2) + (EF1%2)*2 + (EF2%2)*4 + (EF3%2)*8;
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
	return Mux2Bit(sinalDeControle.bits.FontePC, UALResult, SaidaUAL, IR.j.address << 2, 
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
	if(PortaOUPC(UALZero))
		PC = MuxFontePC(UALResult);
}

int BancoDeRegistradores(int *RegATemp, int *RegBTemp) {
	*RegATemp = BCO_REG[IR.r.rs];
	*RegBTemp = BCO_REG[IR.r.rt];

	if(sinalDeControle.bits.EscReg == 1)
		BCO_REG[MuxRegDest()] = MuxMemParaReg();
}

int UAL(int op, int *UALZero) {
	int a = MuxUALFonteA();
	int b = MuxUALFonteB();
	*UALZero = (a - b == 0) ? 1 : 0;
	//printf("a: %d, b: %d\n", a, b);
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

void EscreveNoIR(int instrucao) {
	if (sinalDeControle.bits.IREsc)
		IR.instrucao = instrucao;
}

int OperacaoValida(unsigned int op) {
	if ((op != 0 && op != 35 && op != 43 && op !=  4 && op !=  2 && op != 3 && op != 20
		     && op != 21 && op !=  8 && op != 12 && op !=  5) && estadoAtual >= 1)
		return 0;

	return 1;
}

int CodigoOperacaoValida(unsigned int func) {
	if (IR.r.op != 0)
		return 1;

	if (func != 32 && func != 34 && func != 36 && func != 37 && func != 42 && estadoAtual >= 1)
		return 0;

	return 1;
}

int LeInstrucoesDaEntrada(char *arquivoEntrada) {
	int byteOffset = 0;
	if (arquivoEntrada == NULL)
		return byteOffset;

	FILE *fp = fopen(arquivoEntrada, "rb");
	if (fp == NULL)
		return byteOffset;

	MEMORIA memoria;
	while (fscanf(fp, "%d", &memoria.inteiro) != EOF) {
		if (byteOffset >= TAMANHO_RAM - TAMANHO_PALAVRA)
			return -1;

		for (int i = TAMANHO_PALAVRA; i > 0 ; --i)
			RAM[byteOffset++] = memoria.byte[i-1];
	}

	return byteOffset;
}
void imprimeSaida(){
	printf("PC: %u\t IR: %u\t MDR: %u\n", PC, IR.instrucao, MDR);
	printf("A: %u\t B: %u\t AluOut: %u\n", RegistradorA, RegistradorB, SaidaUAL);

	printf("Banco De Registradores\n");	
	printf("R00(r0)=%d\t R08(t0)=%d\t R16(s0)=%d\t R24(t8)=%d\n", BCO_REG[0], BCO_REG[8],
																  BCO_REG[16], BCO_REG[24]);

	printf("R01(at)=%d\t R09(t1)=%d\t R17(s1)=%d\t R25(t9)=%d\n", BCO_REG[1], BCO_REG[9],
																  BCO_REG[17], BCO_REG[25]);

	printf("R02(v0)=%d\t R10(t2)=%d\t R18(s2)=%d\t R26(k0)=%d\n", BCO_REG[2], BCO_REG[10],
																  BCO_REG[18], BCO_REG[26]);

	printf("R03(v1)=%d\t R11(t3)=%d\t R19(s3)=%d\t R27(k1)=%d\n", BCO_REG[3], BCO_REG[11],
																  BCO_REG[19], BCO_REG[27]);

	printf("R04(a0)=%d\t R12(t4)=%d\t R20(s4)=%d\t R28(gp)=%d\n", BCO_REG[4], BCO_REG[12],
																  BCO_REG[20], BCO_REG[28]);

	printf("R05(a1)=%d\t R13(t5)=%d\t R21(s5)=%d\t R29(sp)=%d\n", BCO_REG[5], BCO_REG[13],
																  BCO_REG[21], BCO_REG[29]);

	printf("R06(a2)=%d\t R14(t6)=%d\t R22(s6)=%d\t R30(fp)=%d\n", BCO_REG[6], BCO_REG[14],
																  BCO_REG[22], BCO_REG[30]);

	printf("R07(a3)=%d\t R15(t7)=%d\t R23(s7)=%d\t R31(ra)=%d\n", BCO_REG[7], BCO_REG[15],
																  BCO_REG[23], BCO_REG[31]);
	MEMORIA memoria;
	printf("Memória (endereços a byte) \n");
	for (int i = 0; i < 8; i++)
	{
		memoria.byte[3] = RAM[(i*4)];
		memoria.byte[2] = RAM[(i*4)+1];
		memoria.byte[1] = RAM[(i*4)+2];
		memoria.byte[0] = RAM[(i*4)+3];
		printf("[%d]%u\t ", i ,memoria.inteiro);

		memoria.byte[3] = RAM[((i+8)*4)];
		memoria.byte[2] = RAM[((i+8)*4)+1];
		memoria.byte[1] = RAM[((i+8)*4)+2];
		memoria.byte[0] = RAM[((i+8)*4)+3];
		printf("[%d]%u\t ", (i+8) ,memoria.inteiro);

		memoria.byte[3] = RAM[((i+16)*4)];
		memoria.byte[2] = RAM[((i+16)*4)+1];
		memoria.byte[1] = RAM[((i+16)*4)+2];
		memoria.byte[0] = RAM[((i+16)*4)+3];
		printf("[%d]%u\t ", (i+16) ,memoria.inteiro);

		memoria.byte[3] = RAM[((i+24)*4)];
		memoria.byte[2] = RAM[((i+24)*4)+1];
		memoria.byte[1] = RAM[((i+24)*4)+2];
		memoria.byte[0] = RAM[((i+24)*4)+3];
		printf("[%d]%u\t \n", (i+24) ,memoria.inteiro);
	}


}

int main(int argc, char *argv[]) {

	if (argc != NARGS) {
		printf("ERRO! Argumentos para utilizar o programa: %s <nome-arquivo-entrada>\n", 
				argv[PROGNAME]);
		return -1;
	}

	InicializaVariaveisGlobais();

	int instrucoes = LeInstrucoesDaEntrada(argv[ARQUIVO_ENTRADA]);
	if (instrucoes <= 0) {
		printf("ERRO! Não foi possível abrir o arquivo de entrada.\n");
		return instrucoes;
	}

	MEMORIA memoria;

	unsigned int IRaux, ULA0;
	unsigned int regA, regB;
	unsigned int ULAres, ULAop;
	do {
		UnidadeDeControle(IR.j.op);
		IRaux = Memoria();
		BancoDeRegistradores(&regA, &regB);
		ULAop = UALcontrole();
		ULAres = UAL(ULAop, &ULA0);
		
		EscreveNoIR(IRaux);
		MDR = IRaux;
		RegistradorA = regA;
		RegistradorB = regB;
		EscreveNoPC(ULAres, ULA0);
		SaidaUAL = ULAres;
		estadoAtual = estadoFuturo;
	} while (OperacaoValida(IR.j.op) && CodigoOperacaoValida(IR.r.funct));

	imprimeSaida();

	return 0;
}
