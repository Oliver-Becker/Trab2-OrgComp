progname=cpu_multi_code

all:
	gcc cpu_multi_code.c -o $(progname) -g

run:
	./$(progname)

zip: 
	zip -r $(progname).zip Makefile cpu_multi_code.c

