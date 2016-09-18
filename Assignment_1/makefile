CC = gcc
CFLAGS = -Wall -O2 -D_REENTRANT -lpthread -lrt
DEPS = func.h
OBJ = func.o
TARGET = ex1

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c $(OBJ) -o $(TARGET)

clean:
	$(RM) $(TARGET)
