EXE_OUTPUT=$(D_ROOT)/build/
CFLAGS=-Wall

all: prime.o

clean:
	rm -f *.o

prime.o: prime.c prime.h
	$(CC) $(CFLAGS) -c prime.c

fastexp.o: fastexp.c fastexp.h
	$(CC) $(CFLAGS) -c fastexp.c