CC=gcc
export CC

all:
	$(MAKE) -C ./lib
	$(MAKE) -C ./test

clean:
	cd ./test && $(MAKE) clean
	cd ./lib && $(MAKE) clean

test:
	cd ./test && $(MAKE) alltests