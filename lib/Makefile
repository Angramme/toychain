EXE_OUTPUT=$(D_ROOT)/build/
CFLAGS=-Wall

OBJECTS = mmath

all: $(OBJECTS:=.o)

clean:
	rm -f $(OBJECTS:=.o)

$(OBJECTS:=.o): %.o: %.c %.h
	$(CC) $(CFLAGS) -c $(word 1,$^)