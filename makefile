main: interface.c
	gcc -g -lm -o interface interface.c

run:
	./interface

go:
	clear
	gcc -g -lm -o interface interface.c
	./interface
