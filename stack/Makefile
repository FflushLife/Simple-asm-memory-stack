.PHONY: all mkdir

BUILD = $(PWD)/build
SRC = $(PWD)/src

all:
	make mkdir
	$(CC) $(SRC)/stack.c $(SRC)/signal_handler.c $(SRC)/sigsegv_test.c -o $(BUILD)/sigsegv_test.o
	$(CC) $(SRC)/stack.c $(SRC)/signal_handler.c $(SRC)/shift_test.c -o $(BUILD)/shift_test.o
	$(CC) $(SRC)/stack.c $(SRC)/signal_handler.c $(SRC)/canary_test.c -o $(BUILD)/canary_test.o

mkdir:
	test -d build || mkdir build/
