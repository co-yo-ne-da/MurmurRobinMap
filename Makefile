LIBRARY_NAME = hashmap
LIBRARY_VERSION = 0.1.0

CC=clang

MURMUR = -I/usr/local/lib/
CFLAGS = -Wall -O3 --std=c99 $(MURMUR)

SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin
DIST_DIR=dist

LIBRARY_FILE = $(DIST_DIR)/lib$(LIBRARY_NAME).a

SOURCES=$(wildcard $(SRC_DIR)/*.c)
OBJECTS=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))


all: $(LIBRARY_FILE)

$(LIBRARY_FILE): $(OBJECTS)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR) $(DIST_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DIST_DIR):
	mkdir $(DIST_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

install:
	cp $(LIBRARY_FILE) /usr/local/lib/
	cp $(SRC_DIR)/$(LIBRARY_NAME).h /usr/local/lib/

uninstall:
	rm -f /usr/local/lib/lib$(LIBRARY_NAME).a
	rm -f /usr/local/lib/$(LIBRARY_NAME).h

clean:
	rm -rf $(DIST_DIR) $(OBJ_DIR)

.PHONY: all clean dist install uninstall
