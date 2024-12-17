# Compiler and flags
CC 			= clang
CXXFLAGS 	= -std=c99 -Wall -Iinclude
LDFLAGS 	= -lraylib
DEBUGFLAGS  = -g -O0
RELEASEFLAGS = -O2

# Source and header files
SRCNAME     = main
SRC 		= src/$(SRCNAME).c
OBJ 		= src/$(SRCNAME).o
HEADERS 	= $(wildcard include/*.h)
TARGET 		= game

# Build the target
.PHONY: all
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJ)
	$(CC) $(CXXFLAGS) $(DEBUGFLAGS) $^ -o $@ $(LDFLAGS)

# Compile each source file into an object file
%.o: %.c $(HEADERS)
	$(CC) $(CXXFLAGS) $(DEBUGFLAGS) -c $< -o $@

# Clean up build artifacts
.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)

