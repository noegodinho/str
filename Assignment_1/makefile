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

all: $(TARGET)

$(TARGET): $(FILE).c
	$(SU) $(CC) $(CFLAGS) $(FILE).c $(CCFLAGS) $(OBJ) -o $(TARGET)

run:
	$(SU) $(TARGET)

clean:
	$(RM) $(TARGET)
