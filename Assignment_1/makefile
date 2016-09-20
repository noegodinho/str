CC = gcc
CFLAGS = -Wall -O2 -D_REENTRANT -lrt
CCFLAGS ?= -lpthread
SOURCE = ./src/
DEPS = ./lib/func.c
OBJ = ./obj/func.o
BIN = ./bin/
FILE = $(SOURCE)ex1
TARGET = $(BIN)ex1
SU = sudo
EXE = $(BIN)ex1
BINDIR= bin
MKDIR = mkdir

all: $(TARGET)

$(TARGET): $(FILE).c
	$(MKDIR) $(BINDIR)
	$(SU) $(CC) $(CFLAGS) $(FILE).c $(CCFLAGS) $(OBJ) -o $(TARGET)

run:
	$(SU) $(EXE)

clean:
	$(RM) $(TARGET)
