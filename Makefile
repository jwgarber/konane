# Jacob Garber and Komal Nandra
# Cmput 497

# `make` creates the executable
# `make filename.o` creates the `filename` object file
# `make clean` will rm all object files and the executable

X ?= 6
Y ?= 6

TARGET = konane

STD = -std=c11 -D_POSIX_C_SOURCE=200809L
#CC = clang
#WARNINGS = -Weverything -Wno-gnu-empty-initializer -Wno-disabled-macro-expansion -Wno-padded
#WARNINGS = -Wall -Wextra -Wpedantic
#SAN = -fsanitize=integer,undefined,address -g
OPTS = -O3 -march=native -flto
CFLAGS = $(STD) $(WARNINGS) $(OPTS) $(SAN)
#LDLIBS = -lpthread

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(SOURCES:.c=$(X)x$(Y).o)

# linking
$(TARGET)$(X)x$(Y): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

# compiling
%$(X)x$(Y).o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -DX=$(X) -DY=$(Y) -c $< -o $@

.PHONY: clean

clean:
	-rm -f *.o $(TARGET)*
