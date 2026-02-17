.PHONY: all clean

CC = gcc
CFLAGS = -Wall -Wextra -O2
SRC = src/main.c src/net.c
OBJ = $(SRC:.c=.o)
TARGET = exec

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)