ifdef PERFTESTS
ADD_CFLAGS=-DPERFTESTS=1
else
ADD_CFLAGS=
endif

ifneq ($(DEBUG),0)
CFLAGS=-Wall -g $(ADD_CFLAGS)
else
CFLAGS=-Wall $(ADD_CFLAGS)
endif
EXE_OUTPUT=$(D_ROOT)/build/tests

TEST_FILES = mmath rsa sign vote dataio blockchain question96 perf
OBJ_FILES = mmath rsa sign vote dataio blockchain


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


$(TEST_FILES): %: %.c test.h $(OBJ_FILES:%=$(D_LIB)/%.o)
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJ_FILES:%=$(D_LIB)/%.o) $(EXTERNAL_LIBS)

