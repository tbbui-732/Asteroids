# Compiler and flags
CC            = clang
CFLAGS        = -std=c99 -Wall -I./lib -I./include
LDFLAGS       = ./lib/libraylib.a raygui.dynlib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
DEBUGFLAGS    = -g -O0
RELEASEFLAGS  = -O2

# Source and header files
SRCNAME       = main
SRC           = src/$(SRCNAME).c
OBJ           = src/$(SRCNAME).o
HEADERS       = $(wildcard include/*.h)
TARGET        = game

# Build the target
.PHONY: all
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $^ $(LDFLAGS) -o $@

# Compile each source file into an object file
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $< -o $@

# Clean up build artifacts
.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)

