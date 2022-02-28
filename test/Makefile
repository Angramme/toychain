CFLAGS=-Wall
EXE_OUTPUT=$(D_ROOT)/build/tests

TEST_FILES = prime fastexp

all:

clean:
	rm -f *.o
	rm -f *.exe

alltests: $(TEST_FILES)
	for ftest in $(TEST_FILES); do \
		echo ==============================================; \
		echo :: running "$$ftest" tests; \
		echo ==============================================; \
		./$$ftest.exe; \
	done

$(TEST_FILES): %: $(D_LIB)/%.o %.c test.h
	$(CC) $(CFLAGS) -o $@ $(D_LIB)/$@.o $@.c

