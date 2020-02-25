# Jacob Garber and Komal Nandra
# Cmput 497

# `make` creates the executable
# `make filename.o` creates the `filename` object file
# `make clean` will rm all object files and the executable

TARGET = konane

STD = -std=c11
CC = clang
WARNINGS = -Weverything -Wno-gnu-empty-initializer -D_POSIX_C_SOURCE=200809L -fsanitize=undefined
#WARNINGS = -Wall -Wextra -Wpedantic
OPTS = -O3 -march=native -flto
CFLAGS = $(STD) $(WARNINGS) $(OPTS)
#LDLIBS = -lpthread

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(SOURCES:.c=.o)

# linking
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

# compiling
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean tar

clean:
	-rm -f $(OBJECTS) $(TARGET)
