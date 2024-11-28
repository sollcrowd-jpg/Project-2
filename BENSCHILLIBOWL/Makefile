# Compiler
CC = gcc

# Compiler flags
CFLAGS = -I. -pthread -std=c99 -Wall

# Dependencies
DEPS = BENSCHILLIBOWL.h

# Object files
OBJ = BENSCHILLIBOWL.o main.o

# Rule to build object files
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Rule to link the executable
main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

# Rule to clean up build artifacts
clean:
	rm -f *.o main
