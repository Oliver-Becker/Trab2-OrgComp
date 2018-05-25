progname=cpu_multi_code

all:
	gcc cpu_multi_code.c -o $(progname)

run:
	./$(progname)

zip: 
	zip -r $(progname).zip Makefile cpu_multi_code.c

