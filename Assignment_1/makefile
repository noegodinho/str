# ****************************************************************
# Autor: José Manuel C. Noronha
# Autor: Noé Godinho
# Turma: PL2
# Grupo: 5
# Ano Lectivo: 2016 - 2017
# ****************************************************************

# Para os exercicios 1,3,4 e 7 deve alterar o campo NAME = exNUMERO(1..7) na linha 22
# Para os exercicios 5 e 6 deve alterar o campo NAME1 = exNUMERO(3 ou 4) na linha 23

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
NAME1 = ex4
FILE = $(SOURCE)$(NAME)
TARGET = $(BIN)$(NAME)
SU = sudo

subsystem:
	$(SU) $(CC) $(CFLAGS) $(FILE).c $(DEPS) $(CCFLAGS) $(OBJ) -o $(TARGET)
	@$(SU) $(TARGET)

run_5_6:
	@$(CC) -c $(SOURCE)ex5.c	# Cria o filcheiro objecto do ex5.c
	@mv -f ex5.o $(OBJ1)			# Move o ex5.o para ./obj/ e muda o nome para func2.o
	$(SU) $(CC) $(CFLAGS) $(SOURCE)$(NAME1).c $(DEPS) $(CCFLAGS) $(OBJ1) -o $(BIN)ex5_6
	@$(SU) $(BIN)ex5_6

run_8_9:
	@$(CC) -c $(SOURCE)ex8.c	# Cria o filcheiro objecto do ex5.c
	@mv -f ex8.o $(OBJ2)				# Move o ex5.o para ./obj/ e muda o nome para func2.o
	$(SU) $(CC) $(CFLAGS) $(SOURCE)ex9.c $(DEPS) $(DEPS1) $(CCFLAGS) $(OBJ1) $(OBJ2) -o $(BIN)ex8_9
	@$(SU) $(BIN)ex8_9

clean:
	$(RM) $(BIN)ex1 $(BIN)ex3 $(BIN)ex4 $(BIN)ex7 $(BIN)ex8_9 $(BIN)ex5_6
