CC = gcc
CFLAGS = -Wall -O3 --std=c99 -I/opt/local/include/ -I/usr/local/lib/ -I$(SRC_DIR)
LDFLAGS = -L/opt/local/lib/ -L/usr/local/lib/ -lcheck -lm -lpthread -lmurmurhash

SRC_DIR = ../src
TEST_DIR = .

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(SRC_DIR)/%.o,$(SRC_FILES))
TEST_RUNNER = $(TEST_DIR)/check_tests.c
TEST_EXECUTABLE = test_suite

check: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)


$(TEST_EXECUTABLE): $(TEST_FILES) $(SRC_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(TEST_EXECUTABLE)

$(TEST_RUNNER): $(TEST_FILES)

.DEFAULT_GOAL := check
