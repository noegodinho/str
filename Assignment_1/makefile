CC = gcc
CFLAGS = -Wall -O2 -D_REENTRANT -lrt -lm
CCFLAGS ?= -lpthread
SOURCE = ./src/
DEPS = ./lib/func.h
OBJ = ./obj/func.o
OBJ1 = ./obj/func2.o
BIN = ./bin/
NAME = ex7
FILE = $(SOURCE)$(NAME)
TARGET = $(BIN)$(NAME)
SU = sudo

subsystem:
	$(SU) $(CC) $(CFLAGS) $(FILE).c $(DEPS) $(CCFLAGS) $(OBJ) -o $(TARGET)
	@$(SU) $(TARGET)

run_5_6:
	@$(CC) -c $(SOURCE)ex5.c	# Cria o filcheiro objecto do ex5.c
	@mv -f ex5.o $(OBJ1)			# Move o ex5.o e muda o nome para func2.o
	$(SU) $(CC) $(CFLAGS) $(SOURCE)ex4.c $(DEPS) $(CCFLAGS) $(OBJ1) -o $(BIN)ex5_6
	@$(SU) $(BIN)ex5_6
