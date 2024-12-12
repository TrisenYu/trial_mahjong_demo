
# SPDX-LICENSE-IDENTIFIER: GPL-2.0
# (C) 2024-11 Author: <kisfg@hotmail.com>
# https://github.com/webassembly/wabt
# https://evian-zhang.github.io/wasm-tutorial/09-%E5%86%85%E5%AD%98%E4%B8%8E%E5%BC%95%E7%94%A8.html

CC:=g++
EMCC=emcc
RM:=rm

EMCC_FLAG:=-s WASM=1 -o
EMCC_OUTCOME:=test.js

TEST_CASES:=tester.exe

DYM_FLAG:=-fpic -shared
DYM_SO:=libmahjong.so
SCRDIR:=


ifeq ($(OS),Windows_NT)
	RM=del
	SCRDIR=$(shell echo %cd%)
else
	SCRDIR=$(shell pwd)
endif
SOURCE:=$(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SOURCE))

Phony:=build
Phony += test
Phony += clean
Phony += help
Phony += exam
Phony += wasm
Phony += dynamico
# $@: 所有目标 $^ 所有依赖
%.o: $(SCRDIR)/%.cpp
	$(CC) -c $^ -o $@

test: $(OBJS) 
	$(CC) $^ -o $(TEST_CASES)
wasm:
	$(EMCC) $(SOURCE) $(EMCC_FLAG) $(EMCC_OUTCOME)
	wasm2wat test.wasm -o test.wat
exam:	
	./$(TEST_CASES)
dynamico:
	$(CC) mahjong.cpp $(DYM_FLAG) -o $(DYM_SO)

Ignore:=clean
clean:
	$(info $(TEST_CASES))
	@$(RM) $(TEST_CASES)
	$(info $(wildcard *.o))
	@$(RM) $(wildcard *.o)
	$(info $(DYM_SO))
	@$(RM) $(DYM_SO)
	$(info $(EMCC_OUTCOME))
	@$(RM) $(EMCC_OUTCOME)
	

help:
	$(info `test` for validating the expectation of the program)
	$(info `help` for de tailed descriptions of the usage of current makefile)
	$(info `clean` for removing binary file)
	$(info `exam` for executing the binary file)
	$(info `wasm` for compiling the webassembly file)
	$(info `dynamico` for generating libmahjong.so for experimental aim)


.PHONY: $(Phony)
.IGNORE: $(Ignore)