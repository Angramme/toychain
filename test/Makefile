CFLAGS=-Wall
EXE_OUTPUT=$(D_ROOT)/build/tests

TEST_FILES = mmath rsa vote

all:

clean:
	rm -f *.o
	rm -f *.exe
	rm -f $(TEST_FILES)

alltests: $(TEST_FILES)
	for ftest in $(TEST_FILES); do \
		echo  ;\
		echo ==============================================; \
		echo :: running "$$ftest" tests; \
		echo ==============================================; \
		echo  ;\
		./$$ftest; \
	done

$(TEST_FILES): %: $(D_LIB)/%.o %.c test.h
	$(CC) $(CFLAGS) -o $@ $@.c $(TEST_FILES:%=$(D_LIB)/%.o)

