LIBRARY_NAME = murmurrobinmap
OUT_LIBRARY_NAME = lib$(LIBRARY_NAME)
LIBRARY_VERSION = 0.1.0

CC ?= clang

CFLAGS = -Wall -O3 --std=c99
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DIST_DIR = dist

ifdef SHARED
	LIBCMD = clang -shared -o
	CFLAGS += -fPIC
	LIBRARY_FILE = $(DIST_DIR)/$(OUT_LIBRARY_NAME).so
else
	LIBCMD = ar rcs
	LIBRARY_FILE = $(DIST_DIR)/$(OUT_LIBRARY_NAME).a
endif

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))


all: $(LIBRARY_FILE)

$(LIBRARY_FILE): $(OBJECTS)
	${LIBCMD} $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR) $(DIST_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DIST_DIR):
	mkdir $(DIST_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

install:
	cp $(LIBRARY_FILE) /opt/local/lib/
	cp $(SRC_DIR)/$(LIBRARY_NAME).h /opt/local/include/

uninstall:
	rm -f /opt/local/lib/$(LIBRARY_NAME).a
	rm -f /opt/local/include/$(LIBRARY_NAME).h

clean:
	rm -rf $(DIST_DIR) $(OBJ_DIR)

help:
	@echo "Usage: "
	@echo "By default 'make' command produces a static library."
	@echo "Use 'SHARED=1 make' to change the target to dynamic .so lib".

.PHONY: all clean dist install uninstall
