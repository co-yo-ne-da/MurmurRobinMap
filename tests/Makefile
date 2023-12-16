CC = gcc
CFLAGS = -Wall -O3 --std=c99 -I/opt/local/include/ -I/usr/local/include/ -I$(SRC_DIR)
LDFLAGS = -L/opt/local/lib/ -L/usr/local/lib/ -lcheck -lm -lpthread

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
    ifneq ($(findstring Ubuntu, $(shell lsb_release -d)),)
        LDFLAGS += -lsubunit
    endif
endif

SRC_DIR = ../src
TEST_DIR = .

TEST_FILES = $(wildcard $(TEST_DIR)/*.c)
TEST_RUNNER = $(TEST_DIR)/check_tests.c
TEST_EXECUTABLE = test_suite

check: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)


$(TEST_EXECUTABLE): $(TEST_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(TEST_EXECUTABLE)

$(TEST_RUNNER): $(TEST_FILES)

.DEFAULT_GOAL := check
