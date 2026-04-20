CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0)

TARGET  = bin/anteater_chess
SRC     = src/main.c src/gui.c src/board.c src/moves.c src/rules.c src/clock.c src/controller.c
OBJ     = $(patsubst src/%.c, bin/%.o, $(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

tar: clean
	mkdir -p Chess_Alpha_src
	cp -r README.md COPYRIGHT.md INSTALL.md Makefile bin/ doc/ src/ pieces/ Chess_Alpha_src/
	tar -czvf Chess_Alpha_src.tar.gz Chess_Alpha_src/
	rm -rf Chess_Alpha_src

tar_user: all
	mkdir -p Chess_Alpha
	cp -r README.md COPYRIGHT.md INSTALL.md bin/ doc/ pieces/ Chess_Alpha/
	tar -czvf Chess_Alpha.tar.gz Chess_Alpha/
	rm -rf Chess_Alpha

test: all
	./$(TARGET)

.PHONY: all clean tar tar_user test