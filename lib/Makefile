EXE_OUTPUT=$(D_ROOT)/build/
CFLAGS=-Wall

OBJECTS = mmath rsa

all: $(OBJECTS:=.o)

clean:
	rm -f $(OBJECTS:=.o)

$(OBJECTS:=.o): %.o: %.c %.h
	$(CC) $(CFLAGS) -c $(word 1,$^)