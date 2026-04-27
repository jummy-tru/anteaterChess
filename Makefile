CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0)

TARGET  = bin/anteater_chess
SRC     = src/main.c src/gui.c src/board.c src/moves.c src/rules.c src/clock.c src/controller.c src/bots.c
OBJ     = $(patsubst src/%.c, bin/%.o, $(SRC))

all: dirs $(TARGET)

dirs: 
	mkdir -p bin pieces

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
	rm -f src/*.o src/*.exe src/*.out
	rm -f *.exe *.out

tar: clean
	mkdir -p Chess_V1.0_src/bin
	cp -r README.md COPYRIGHT.md INSTALL.md Makefile doc/ src/ pieces/ Chess_V1.0_src/
	tar -czvf Chess_V1.0_src.tar.gz Chess_V1.0_src/
	rm -rf Chess_V1.0_src

tar_user: all
	mkdir -p Chess_V1.0/bin
	cp $(TARGET) Chess_V1.0/bin
	cp -r README.md COPYRIGHT.md INSTALL.md doc/ pieces/ Chess_V1.0/
	tar -czvf Chess_V1.0.tar.gz Chess_V1.0/
	rm -rf Chess_V1.0

test: all
	./$(TARGET)

.PHONY: all clean tar tar_user test