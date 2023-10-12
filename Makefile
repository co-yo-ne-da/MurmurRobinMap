CC=clang

MURMUR = -I/usr/local/lib/ -lmurmurhash

CFLAGS = -Wall -O3 --std=c99 $(MURMUR)

SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

SOURCES=$(wildcard $(SRC_DIR)/*.c)
OBJECTS=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

BIN = $(BIN_DIR)/main

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -r $(BIN_DIR)/* $(OBJ_DIR)/*

run:
	$(BIN)

asm:
	otool -tvV $(BIN)
