ifneq ($(DEBUG),0)
CFLAGS=-Wall -g
else
CFLAGS=-Wall
endif
EXE_OUTPUT=$(D_ROOT)/build/tests

TEST_FILES = mmath rsa sign vote dataio blockchain


all:

clean:
	rm -f *.o
	rm -f *.exe
	rm -f $(TEST_FILES)
	rm -f *.txt


ifneq ($(VALGRIND),0)
alltests: $(TEST_FILES)
	for ftest in $(TEST_FILES); do \
		echo ==============================================; \
		echo :: running "$$ftest" tests with valgrind; \
		echo ==============================================; \
		valgrind --leak-check=full ./$$ftest; \
	done
else
alltests: $(TEST_FILES)
	for ftest in $(TEST_FILES); do \
		echo ==============================================; \
		echo :: running "$$ftest" tests; \
		echo ==============================================; \
		./$$ftest; \
	done
endif


$(TEST_FILES): %: $(D_LIB)/%.o %.c test.h
	$(CC) $(CFLAGS) -o $@ $@.c $(TEST_FILES:%=$(D_LIB)/%.o) $(EXTERNAL_LIBS)

