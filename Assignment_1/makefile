CC = gcc
CFLAGS = -Wall -O2 -D_REENTRANT -lrt -lm
CCFLAGS ?= -lpthread
SOURCE = ./src/
DEPS = ./lib/func.h
DEPS1 = ./lib/ex8.h
OBJ = ./obj/func.o
OBJ1 = ./obj/func2.o
OBJ2 = ./obj/ex8.o
BIN = ./bin/
NAME = ex7
FILE = $(SOURCE)$(NAME)
TARGET = $(BIN)$(NAME)
SU = sudo
NAME1 = ex4.c	# Deve alterar aqui o nome para ex3 ou ex4, refernte ao exercicios 5 e 6

subsystem:
	$(SU) $(CC) $(CFLAGS) $(FILE).c $(DEPS) $(CCFLAGS) $(OBJ) -o $(TARGET)
	@$(SU) $(TARGET)

run_5_6:
	@$(CC) -c $(SOURCE)ex5.c	# Cria o filcheiro objecto do ex5.c
	@mv -f ex5.o $(OBJ1)			# Move o ex5.o para ./obj/ e muda o nome para func2.o
	$(SU) $(CC) $(CFLAGS) $(SOURCE)$(NAME1) $(DEPS) $(CCFLAGS) $(OBJ1) -o $(BIN)ex5_6
	@$(SU) $(BIN)ex5_6

run_8_9:
	@$(CC) -c $(SOURCE)ex8.c	# Cria o filcheiro objecto do ex5.c
	@mv -f ex8.o ./obj/				# Move o ex5.o para ./obj/ e muda o nome para func2.o
	$(SU) $(CC) $(CFLAGS) $(SOURCE)ex9.c $(DEPS) $(DEPS1) $(CCFLAGS) $(OBJ1) $(OBJ2) -o $(BIN)ex8_9
	@$(SU) $(BIN)ex8_9
