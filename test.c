#include <stdio.h>
#include <stdlib.h>

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

struct sc {
	unsigned RegDest:2;
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
};

union controle {
	unsigned sinal;
	struct sc cont;
};

union memoria {
	unsigned int bytes;
	unsigned char byte[4];
};

int ExtensaoDeSinal(short int valor16Bits) {
	return (int)valor16Bits;
}

int main() {

	TIPO_R tr;
	printf("tamanho tipo_r: %zd\n", sizeof(tr));

/*	union u k;
	k.intero = 32;
	printf("intero: %d\n", k.intero);
	printf("1 bit: %d\n", k.bit.a);
	printf("2 bit: %d\n", k.bit.b);
	printf("3 bit: %d\n", k.bit.c);
*/
	INSTRUCAO i;
	i.instrucao = 0x2042FFFF;
//	scanf("%x", &i.instrucao);

	printf("\ntipo r:\n");
	printf("op: %x\n", i.r.op);
	printf("rs: %x\n", i.r.rs);
	printf("rt: %x\n", i.r.rt);
	printf("rd: %x\n", i.r.rd);
	printf("shamt: %x\n", i.r.shamt);
	printf("funct: %x\n", i.r.funct);

	printf("\ntipo i:\n");
	printf("op: %x\n", i.i.op);
	printf("rs: %x\n", i.i.rs);
	printf("rt: %x\n", i.i.rt);
	printf("immediate: %x\n", i.i.immediate);

	printf("\ntipo j:\n");
	printf("op: %x\n", i.j.op);
	printf("address: %x\n", i.j.address);

	printf("\n\n");
	printf("int imm: %d\n", i.i.immediate);
	printf("short imm: %d\n", ExtensaoDeSinal(i.i.immediate));
	printf("shifted int imm: %d\n", i.i.immediate << 2);
	printf("shifted short imm: %d\n", ExtensaoDeSinal(i.i.immediate) << 2);

	printf("\n\n");

	int imm = 0xFFFF;
	printf("int imm: %d\n", imm);
	printf("short imm: %d\n", (short int)imm);

	short int shimm = imm;
	printf("short shimm: %hd\n", imm);

	return 0;
}
