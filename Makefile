# Jacob Garber and Komal Nandra
# Cmput 497

# `make` creates the executable
# `make filename.o` creates the `filename` object file
# `make clean` will rm all object files and the executable

SIZE ?= 6

TARGET = konane

STD = -std=c11 -D_POSIX_C_SOURCE=200809L
CC = clang
WARNINGS = -Weverything -Wno-gnu-empty-initializer -Wno-disabled-macro-expansion -fsanitize=undefined,integer
#WARNINGS = -Wall -Wextra -Wpedantic
OPTS = -O3 -march=native -flto
CFLAGS = $(STD) $(WARNINGS) $(OPTS)
#LDLIBS = -lpthread

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(SOURCES:.c=$(SIZE).o)

# linking
$(TARGET)$(SIZE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

# compiling
%$(SIZE).o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -DSIZE=$(SIZE) -c $< -o $@

.PHONY: clean

clean:
	-rm -f *.o $(TARGET)*
