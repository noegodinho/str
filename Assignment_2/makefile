# ****************************************************************
# Autor: José Manuel C. Noronha
# Autor: Noé Godinho
# Turma: PL2
# Grupo: 5
# Ano Lectivo: 2016 - 2017
# ****************************************************************


CC = gcc
CFLAGS = -Wall -O2 -D_REENTRANT
CCFLAGS ?= -lpthread -lm -lrt
SOURCE = ./src/
BIN = ./bin/
NAME = ex6
FILE = $(SOURCE)$(NAME)
TARGET = $(BIN)$(NAME)
SU = sudo
CLR = clear

subsystem:
	make clean
	@$(CLR)
	@$(CLR)
	$(SU) $(CC) $(CFLAGS) $(FILE).c $(CCFLAGS) -o $(TARGET)
	@$(SU) $(TARGET) < examples_t.txt

clean:
	$(RM) $(BIN)*
