# Define the compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -pthread -std=c99 -D_XOPEN_SOURCE=700

# Target executable name
TARGET = shm_processes

# Default rule to compile the program
all: $(TARGET)

# Rule to create the executable
$(TARGET): shm_processes.c
	$(CC) $(CFLAGS) -o $(TARGET) shm_processes.c

# Rule to clean up generated files
clean:
	rm -f $(TARGET)
