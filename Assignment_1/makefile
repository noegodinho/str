CC = gcc
CFLAGS = -Wall -O2 -D_REENTRANT -lrt -lm
CCFLAGS ?= -lpthread
SOURCE = ./src/
DEPS = ./lib/func.h
OBJ = ./obj/func.o
BIN = ./bin/
NAME = ex1
FILE = $(SOURCE)$(NAME)
TARGET = $(BIN)$(NAME)
SU = sudo

all: $(TARGET)

$(TARGET): $(FILE).c
	$(SU) $(CC) $(CFLAGS) $(FILE).c $(DEPS) $(CCFLAGS) $(OBJ) -o $(TARGET)

run:
	$(SU) $(TARGET)

clean:
	$(RM) $(TARGET)
