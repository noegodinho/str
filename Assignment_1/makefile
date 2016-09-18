CC = gcc
CFLAGS = -Wall -O2 -D_REENTRANT -lpthread -lrt
SOURCE = ./src/
DEPS = ./lib/func.c
OBJ = ./obj/func.o
BIN = ./bin/
FILE = $(SOURCE)ex1
TARGET = $(BIN)ex1

all: $(TARGET)

$(TARGET): $(FILE).c
	$(CC) $(CFLAGS) $(FILE).c $(OBJ) -o $(TARGET)

clean:
	$(RM) $(TARGET)
