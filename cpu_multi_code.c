/*
Turma 2
Clara Rosa Silveira                9021070            - Unidade de controle, Alterações na MEF
Gabriela Isabel Chavez Estevez     10295440           - Unidade de controle, Alterações na MEF
Rafael Farias Roque                10295412           - Caminho de dados, Alterações na MEF
Óliver Savastano Becker            10284890           - Formatação das variaveis e Caminho de dados
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_RAM 256
#define TAMANHO_PALAVRA 4
#define QUANTIDADE_REGISTRADORES 32

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
int BCO_REG[QUANTIDADE_REGISTRADORES];
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

	unsigned int destino = MuxRegDest();
	int conteudo = MuxMemParaReg();

	if(sinalDeControle.bits.EscReg == 1) {
		if (destino == 0) 	// Tentativa de escrever no registrador $zero (proibido)
			return 1 << 4;

		BCO_REG[destino] = conteudo;
	}

	return 0;
}

int UAL(int op, int *UALZero) {
	int a = MuxUALFonteA();
	int b = MuxUALFonteB();
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

int LePalavraDaMemoria(unsigned int byteOffset, int* palavraLida) {
	if (byteOffset > TAMANHO_RAM - TAMANHO_PALAVRA)
		return 1 << 2;

	MEMORIA memoria;

	for (int i = TAMANHO_PALAVRA; i > 0 ; --i)
		 memoria.byte[i-1] = RAM[byteOffset++];

	*palavraLida = memoria.inteiro;

	return 0;
}

int EscrevePalavraNaMemoria(unsigned int byteOffset, unsigned int palavra) {
	if (byteOffset > TAMANHO_RAM - TAMANHO_PALAVRA)
		return 1 << 2;

	MEMORIA memoria;
	memoria.inteiro = palavra;

	for (int i = TAMANHO_PALAVRA; i > 0 ; --i)
		RAM[byteOffset++] = memoria.byte[i-1];

	return 0;
}

int Memoria(int* palavraLida) {
	unsigned int endereco = MuxIouD();

	if(sinalDeControle.bits.LerMem)
		return LePalavraDaMemoria(endereco, palavraLida);

	else if(sinalDeControle.bits.EscMem)
		return EscrevePalavraNaMemoria(endereco, RegistradorB);

	return 0;
}

void EscreveNoIR(int instrucao) {
	if (sinalDeControle.bits.IREsc)
		IR.instrucao = instrucao;
}

int OperacaoValida(unsigned int op) {
	if ((op != 0 && op != 35 && op != 43 && op !=  4 && op !=  2 && op != 3 && op != 20
		     && op != 21 && op !=  8 && op != 12 && op !=  5) && estadoAtual >= 1)
		return 1;

	return 0;
}

int CampoDeFuncaoValido(unsigned int func) {
	if (IR.r.op != 0)
		return 0;

	if (func != 32 && func != 34 && func != 36 && func != 37 && func != 42 && estadoAtual >= 1)
		return 1 << 3;

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
	while (fscanf(fp, "%d", &memoria.inteiro) != EOF) {
		if (byteOffset >= TAMANHO_RAM - TAMANHO_PALAVRA)
			return -1;

		for (int i = TAMANHO_PALAVRA; i > 0 ; --i)
			RAM[byteOffset++] = memoria.byte[i-1];
	}

	return byteOffset;
}

void imprimeSaida(int ERROR) {

	printf("Status da Saída: ");
	if (ERROR & 1)
		printf("Término devido à tentativa de execução de instrução inválida.\n\n");
	else if (ERROR >> 2 & 1)
		printf("Término devido a acesso inválido de memória.\n\n");
	else if (ERROR >> 3 & 1)
		printf("Término devido à operação inválida da ULA.\n\n");
	else if (ERROR >> 4 & 1)
		printf("Término devido a acesso inválido ao Banco De Registradores.\n\n");

	printf("PC=%u\t IR=%u\t MDR=%u\n", PC, IR.instrucao, MDR);
	printf("A=%u\t B=%u\t AluOut=%u\n", RegistradorA, RegistradorB, SaidaUAL);
	printf("Controle=%d\n\n", sinalDeControle);

	char nomeRegistrador[QUANTIDADE_REGISTRADORES][3] = {"r0", "at", "v0", "v1", "a0", "a1",
		"a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2",
		"s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"};

	printf("Banco De Registradores\n");	
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 4; ++j) {
			int reg = i + (j * 8);
			printf("R%02d(%2s)=%d\t", reg, nomeRegistrador[reg], BCO_REG[reg]);
		}
		printf("\n");
	}

	printf("\n");

	int conteudoMemoria;
	printf("Memória (endereços a byte) \n");
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 4; ++j) {
			int posMemoria = (i * 4) + (j * 8 * 4);
			LePalavraDaMemoria(posMemoria, &conteudoMemoria);
			printf("[%02d]=%-10u ", posMemoria, conteudoMemoria);
		}
		printf("\n");
	}

}

int main(int argc, char *argv[]) {

	if (argc != NARGS) {
		printf("ERRO! Argumentos para utilizar o programa: %s <nome-arquivo-entrada>\n", 
				argv[PROGNAME]);
		return -1;
	}

	InicializaVariaveisGlobais();

	if (!LeInstrucoesDaEntrada(argv[ARQUIVO_ENTRADA])) {
		printf("ERRO! Não foi possível abrir o arquivo de entrada.\n");
		return -2;
	}

	MEMORIA memoria;

	int erro;

	unsigned int IRaux, ULA0;
	unsigned int regA, regB;
	unsigned int ULAres, ULAop;
	do {
		erro = 0;
		// Chama as unidades funcionais para executarem.
		UnidadeDeControle(IR.j.op);
		erro += Memoria(&IRaux);
		erro += BancoDeRegistradores(&regA, &regB);
		ULAop = UALcontrole();
		ULAres = UAL(ULAop, &ULA0);
		
		// Atribui os valores aos registradores globais, agora que o ciclo acabou;
		EscreveNoIR(IRaux);
		MDR = IRaux;
		RegistradorA = regA;
		RegistradorB = regB;
		EscreveNoPC(ULAres, ULA0);
		SaidaUAL = ULAres;
		estadoAtual = estadoFuturo;

		// Confere se a operação e o campo de função são validos.
		erro += OperacaoValida(IR.j.op);
		erro += CampoDeFuncaoValido(IR.r.funct);
	} while (!erro);

	imprimeSaida(erro);

	return 0;
}
