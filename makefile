
# SPDX-LICENSE-IDENTIFIER: GPL-2.0
# (C) 2024-11 Author: <kisfg@hotmail.com>
CC:=g++
LDFLAG:=-o

TARGET:=mahjong.exe
TEST_CASES:=tester.exe

SOURCE:=$(wildcard *.cpp *.h)
RM:=rm

ifeq ($(OS),Windows_NT)
	PWD=@echo %cd%
	RM=del
else
	PWD=pwd
endif

PHONY=build
PHONY += test
PHONY += clean
PHONY += help
PHONY += run_test

test:
	$(CC) $(SOURCE) $(LDFLAG) $(TEST_CASES)

run_test:	
	./$(TEST_CASES)

clean:
	@$(RM) $(TEST_CASES)

help:
	@echo `test` for validating the expectation of the program
	@echo `help` for detailed description of the usage of this makefile
	$(info `clean` for removing binary file)
	$(info `run_test` for executing the binary file)

.PHONY: $(PHONY)