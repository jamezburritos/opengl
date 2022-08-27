SHELL  := /bin/sh
CC     := clang
CFLAGS := `pkg-config --libs glfw3 opengl x11 cglm`

OUTFILE := build.out

SRC := src
OBJ := obj
INC := include

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c -I$(INC) -o $@ $<

.PHONY: build run clean

build: $(OBJECTS)
	$(CC) $(CFLAGS) -I$(INC) $^ -o $(OUTFILE)

run:
	./$(OUTFILE)

clean:
	rm -f $(OBJ)/**
