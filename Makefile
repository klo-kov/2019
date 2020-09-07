Automata : main.o define.o
	gcc -o Automata main.o define.o

define.o : define.h define.c
	gcc -c define.c

main.o : define.h main.c
	gcc -c main.c

clean :
	rm *.o
	rm Automata
