all: shell.o
	gcc shell.o

run:
	./a.out

shell.o:
	gcc -c shell.c

clean:
	rm -rf ./a.out *.o *.txt
