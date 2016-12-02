# ****************************************************************
# Autor: José Manuel C. Noronha
# Autor: Noé Godinho
# Turma: PL2
# Grupo: 5
# Ano Lectivo: 2016 - 2017
# ****************************************************************


CC = gcc
CFLAGS = -Wall -O2 -D_REENTRANT -lrt
CCFLAGS ?= -lpthread -lm
SOURCE = ./src/
BIN = ./bin/
NAME = ex3
FILE = $(SOURCE)$(NAME)
TARGET = $(BIN)$(NAME)
SU = sudo

subsystem:
	$(SU) $(CC) $(CFLAGS) $(FILE).c $(CCFLAGS) -o $(TARGET)
	@$(SU) $(TARGET) < examples2.txt

clean:
	$(RM) $(BIN)ex1 $(BIN)ex2 $(BIN)ex3
