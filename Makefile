CC=gcc
export CC

# current working direcotry
ROOTD=$(shell pwd)
C_INCLUDE_PATH=$(ROOTD)
export ROOTD
export C_INCLUDE_PATH

all:
	$(MAKE) -C ./lib
	$(MAKE) -C ./test

clean:
	cd ./test && $(MAKE) clean
	cd ./lib && $(MAKE) clean

.PHONY: test
test:
	cd ./test && $(MAKE)  alltests