CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0)

TARGET  = bin/anteater_chess
SRC     = src/main.c src/gui.c src/board.c src/moves.c src/rules.c src/clock.c src/controller.c
OBJ     = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean