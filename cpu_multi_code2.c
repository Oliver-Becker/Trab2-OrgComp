#include <stdio.h>
#include <stdlib.h>
#define TAM 256

int PC;
int IR;
int SaidaUAL;
int BCO_REG[32];
char estadoaAtual;
char estadoFuturo;
unsigned int bitsDeControle;
unsigned char RAM[TAM];

void InicializaVariaveisGlobais(){
	IR=0;
	PC=0;
	controle=0;
	estadoatual = 0;
	estadoFuturo = 0;
	for(int i=0; i<TAM; i++)
		RAM[i]=0;
	for(int i=0; i<32; i++)
		BCO_REG[i]=0;
}


void UnidadeDeControle(char codOp){
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

	EF3 = (!EA3 && !EA2 && !EA1 && EA0 && !OP5 && !OP4 && !OP3 && OP2 && !OP1 && !OP0) || (!EA3 && 
	!EA2 && !EA1 && EA0 && !OP5 && !OP4 && !OP3 && !OP2 && OP1 && !OP0) || (!EA3 && !EA2 && !EA1 && 
	EA0 && !OP5 && !OP4 && !OP3 && !OP2 && OP1 && OP0) || (!EA3 && !EA2 && !EA1 && EA0 && !OP5 && 
	OP4 && !OP3 && OP2 && !OP1 && OP0) ||(!EA3 && !EA2 && !EA1 && EA0 && !OP5 && OP4 && !OP3 && OP2
	&& !OP1 && !OP0) || (!EA3 && !EA2 && !EA1 && EA0 && !OP5 && !OP4 && OP3 && OP2 && !OP1 && !OP0) 
	|| (!EA3 && !EA2 && !EA1 && EA0 && !OP5 && !OP4 && !OP3 && OP2 && !OP1 && OP0) || (!EA3 && !EA2 
	&& EA1 && !EA0 && !OP5 && !OP4 && OP3 && !OP2 && !OP1 && !OP0) || (EA3 && EA2 && !EA1 && EA0);

	EF2 = (!EA3 && !EA2 && !EA1 && EA0 && !OP5 && !OP4 && !OP3 && !OP2 && !OP1 && !OP0) || (!EA3 && 
	!EA2 && !EA1 && EA0 && !OP5 && OP4 && !OP3 && OP2 && !OP1 && !OP0) || (!EA3 && !EA2 && !EA1 && 
	EA0 && !OP5 && !OP4 && OP3 && OP2 && !OP1 && !OP0) || (!EA3 && !EA2 && !EA1 && EA0 && !OP5 && 
	!OP4 && !OP3 && OP2 && !OP1 && OP0) || (!EA3 && !EA2 && EA1 && !EA0 && OP5 && !OP4 && OP3 && 
	!OP2 && OP1 && OP0) || (!EA3 && !EA2 && EA1 && !EA0 && !OP5 && !OP4 && OP3 && !OP2 && !OP1 && 
	!OP0) || (!EA3 && !EA2 && EA1 && EA0) || (!EA3 && EA2 && EA1 && !EA0) || (EA3 && EA2 && !EA1 && 
	EA0);

	EF1 = (!EA3 && !EA2 && !EA1 && EA0 && OP5 && !OP4 && !OP3 && !OP2 && OP1 && OP0) || (!EA3 && !EA2 
	&& !EA1 && EA0 && OP5 && !OP4 && OP3 && !OP2 && OP1 && OP0) || (!EA3 && !EA2 && !EA1 && EA0 && 
	!OP5 && !OP4 && OP3 && !OP2 && !OP1 && !OP0) || (!EA3 && !EA2 && !EA1 && EA0 && !OP5 && !OP4 && 
	!OP3 && !OP2 && !OP1 && !OP0) ||(!EA3 && !EA2 && !EA1 && EA0 && !OP5 && !OP4 && !OP3 && !OP2 && 
	OP1 && OP0) || (!EA3 && !EA2 && !EA1 && EA0 &&!OP5 && OP4 && !OP3 && OP2 && !OP1 && OP0) || (!EA3 
	&& !EA2 && !EA1 && EA0 && !OP5 && !OP4 && !OP3 && OP2 && !OP1 && OP0) || (!EA3 && !EA2 && EA1 &&
	!EA0 && OP5 && !OP4 && !OP3 && !OP2 && OP1 && OP0) || (!EA3 && !EA2 && EA1 && !EA0 && !OP5 && 
	!OP4 && OP3 && !OP2 && !OP1 && !OP0) || (!EA3 && EA2 && EA1 && !EA0);

	EF0 = (!EA3 && !EA2 && !EA1 && !EA0) || (!EA3 && !EA2 && !EA1 && EA0 && !OP5 && !OP4 && !OP3 && 
	!OP2 && OP1&& !OP0) || (!EA3 && !EA2 && !EA1 && EA0 && !OP5 && OP4 && !OP3 && OP2 && !OP1 && OP0)
	|| (!EA3 && !EA2 && !EA1 && EA0 && !OP5 && !OP4 && OP3 && OP2 && !OP1 && !OP0) || (!EA3 && !EA2
	&& !EA1 && EA0 && !OP5 && !OP4 &&!OP3 && OP2 && !OP1 && OP0) || (!EA3 && !EA2 && EA1 && !EA0 && 
	OP5 && !OP4 && !OP3 && !OP2 && OP1 && OP0) || (!EA3 && !EA2 && EA1 && !EA0 && OP5 && !OP4 && OP3
	&& !OP2 && OP1 && OP0) || (!EA3 && EA2 && EA1 && !EA0) || (EA3 && EA2 && !EA1 && EA0);

	MemParaReg1 = (EA3 && !EA2 && EA1 && !EA0) || (EA3 && !EA2 && EA1 && EA0);

	MemParaReg0 = (!EA3 && EA2 && !EA1 && !EA0);

	IREsc = (!EA3 && !EA2 && !EA1 && !EA0);

	BNE = (!EA3 && !EA2 && !EA1 && !EA0);

	EEAcMem = (!EA3 && EA2 && !EA1 && EA0);

	LerMem = (!EA3 && !EA2 && !EA1 && !EA0) || (!EA3 && !EA2 && EA1 && EA0);

	IouD = (!EA3 && !EA2 && EA1 && EA0) || (!EA3 && EA2 && !EA1 && EA0);

	PCEsc = (!EA3 && !EA2 && !EA1 && !EA0) || (EA3 && !EA2 && !EA1 && EA0) || (EA3 && !EA2 && EA1 &&
	!EA0) || (EA3 && !EA2 && EA1 && EA0) || (EA3 && EA2 && !EA1 && !EA0);

	PcEscCond = (EA3 && !EA2 && !EA1 && !EA0) || (EA3 && EA2 && EA1 && EA0);

	FontePC1 = (EA3 && !EA2 && EA1 && !EA0) || (EA3 && !EA2 && EA1 && EA0) || (EA3 && EA2 && !EA1 &&
	!EA0);

	FontePC0 = (EA3 && !EA2 && !EA1 && !EA0) || (EA3 && !EA2 && EA1 && EA0) || (EA3 && EA2 && !EA1 &&
	!EA0) || (EA3 && EA2 && EA1 && EA0);

	UALOp1 = (!EA3 && EA2 && EA1 && !EA0) || (EA3 && EA2 && !EA1 && EA0);

	UALOp0 = (EA3 && !EA2 && !EA1 && !EA0) || (EA3 && EA2 && !EA1 && EA0) || (EA3 && EA2 && EA1 && EA0);

	UALFonteB1 = !((!EA3 && !EA2 && !EA1 && !EA0) || (!EA3 && EA2 && EA1 && !EA0) || (EA3 && !EA2 &&
	!EA1 && !EA0) || (EA3 && EA2 && EA1 && EA0));

	UALFonteB0 = (!EA3 && !EA2 && !EA1 && !EA0) || (!EA3 && !EA2 && !EA1 && EA0 && OP5 && !OP4 && 
	!OP3 && !OP2 && OP1 && OP0) || (!EA3 && !EA2 && !EA1 && EA0 && OP5 && !OP4 && OP3 && !OP2 && OP1
	&& OP0) || (!EA3 && !EA2 && !EA1 && EA0 && !OP5 && !OP4 && OP3 && !OP2 && !OP1 && !OP0) || (!EA3
	&& !EA2 && !EA1 && EA0 && !OP5 && !OP4 && !OP3 && !OP2 && !OP1 && !OP0) || (!EA3 && !EA2 && !EA1
	&& EA0 && !OP5 && !OP4 && !OP3 && OP2 && !OP1 && !OP0) || (!EA3 && !EA2 && !EA1 && EA0 && !OP5 &&
	!OP4 && !OP3 && !OP2 && OP1 && !OP0) || (!EA3 && !EA2 && !EA1 && EA0 && !OP5 && !OP4 && !OP3 && 
	!OP2 && OP1 && OP0) || (!EA3 && !EA2 && !EA1 && EA0 && !OP5 && OP4 && !OP3 && OP2 && !OP1 && OP0)
	|| (!EA3 && !EA2 && !EA1 && EA0 && !OP5 && OP4 && !OP3 && OP2 && !OP1 && !OP0) || (!EA3 && !EA2 
	&& !EA1 && EA0 && !OP5 && !OP4 && OP3 && OP2 && !OP1 && !OP0) || (!EA3 && !EA2 && !EA1 && EA0 && 
	!OP5 && !OP4 && !OP3 && OP2 && !OP1 && OP0);

	UALFonteA = (!EA3 && !EA2 && EA1 && !EA0 && OP5 && !OP4 && !OP3 && !OP2 && OP1 && OP0) || (!EA3 
	&& !EA2 && EA1 && !EA0 && OP5 && !OP4 && OP3 && !OP2 && OP1 && OP0) || (!EA3 && !EA2 && EA1 && 
	!EA0 && !OP5 && !OP4 && OP3 && !OP2 && !OP1 && !OP0) || (!EA3 && EA2 && EA1 && !EA0) || (EA3 && 
	!EA2 && !EA1 && !EA0) || (EA3 && EA2 && EA1 && EA0) || (EA3 && EA2 && !EA1 && EA0) 

	EscReg = (!EA3 && EA2 && !EA1 && !EA0) || (!EA3 && EA2 && EA1 && EA0) || (EA3 && !EA2 && EA1 &&
 	!EA0) || (EA3 && !EA2 && EA1 && EA0) || (EA3 && EA2 && EA1 && !EA0);

	RegDst1 = (EA3 && !EA2 && EA1 && !EA0);

	RegDst0 = (!EA3 && EA2 && EA1 && EA0);

	bitsDeControle = RegDst0 + RegDst1*2 + EscReg*4 + UALFonteA*8 + UALFonteB0*16 + UALFonteB1*32 +
	UALOp0*64 * UALOp1*128 + FontePC0*256 + FontePC1*512 + PcEscCond*1024 + PCEsc*2048 + IouD*4096 +
	LerMem*8192 + EscMem*16384 + BNE*32768 + IREsc*65536 + MemParaReg0*131072 + MemParaReg1*262144;

	estadoFuturo = EF0 + EF1*2 + EF2*4 + EF3*8;	
} 


void UALcontrole(){

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

int main(int argc, char const *argv[]){

	FILE *fp;
	fp = fopen("code.bin", "r");
	if(fp==NULL){
		printf("ERROR\n");		
		exit(0);
	}

	inicializa();

	int count=0;
	fread(&RAM[count], sizeof(unsigned char), 1, fp);	
	while(!feof(fp)){ //informacoes do arquivo code.bin passadas para RAM
		count++;
		fread(&RAM[count], sizeof(unsigned char), 1, fp);
	}

	count=0;
	while(PC < count){
		UC();
		UALcontrole();
		UAL();
		BR();
		Memoria();
		Instrucao();
		MDR();
	}

	fclose(fp);

	return 0;
}

