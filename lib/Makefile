ifneq ($(DEBUG),0)
CFLAGS=-Wall -g
else
CFLAGS=-Wall
endif
EXE_OUTPUT=$(D_ROOT)/build/

OBJECTS = mmath rsa sign dataio vote

all: $(OBJECTS:=.o)

clean:
	rm -f $(OBJECTS:=.o)

$(OBJECTS:=.o): %.o: %.c %.h
	$(CC) $(CFLAGS) -c $(word 1,$^)