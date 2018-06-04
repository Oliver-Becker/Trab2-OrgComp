progname=cpu_multi_code
entrada=code.bin

all:
	gcc cpu_multi_code.c -o $(progname) -g

run:
	./$(progname) $(entrada)

zip: 
	zip -r $(progname).zip Makefile cpu_multi_code.c

