CFLAGS=-Wall
D_LIB=../lib
EXE_OUTPUT=../build/tests


all: prime

clean:
	rm *.o

alltests: prime fastexp
	./prime
	./fastexp

prime: $(D_LIB)/prime.o prime.c test.h
	$(CC) $(CFLAGS) -o prime $(D_LIB)/prime.o prime.c
	# ln -s -t prime $(EXE_OUTPUT)/prime

fastexp: $(D_LIB)/fastexp.o fastexp.c test.h
	$(CC) $(CFLAGS) -o fastexp $(D_LIB)/fastexp.o fastexp.c

