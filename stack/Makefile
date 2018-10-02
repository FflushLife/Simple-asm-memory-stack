.PHONY: all mkdir

BUILD = $(PWD)/build
SRC = $(PWD)/src

all:
	make mkdir
	$(CC) $(SRC)/stack.c $(SRC)/signal_handler.c $(SRC)/test.c -o $(BUILD)/test.o

mkdir:
	test -d build || mkdir build/
