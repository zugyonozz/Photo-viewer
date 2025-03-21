# Compiler and flags
CXX = g++
CC = gcc
CXXFLAGS = -Wall -Wextra -g -std=c++17
CFLAGS = -Wall -Wextra -g
INCLUDES = -I./include
LDFLAGS = -L./lib
LDLIBS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

# Directories
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = bin/obj

# Files
SRCS_CPP = $(wildcard $(SRC_DIR)/*.cpp)
OBJS_CPP = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS_CPP))
OBJS = $(OBJS_CPP)
TARGET = $(BIN_DIR)/View.exe

# Create obj directory if it doesn't exist
$(shell mkdir -p $(OBJ_DIR))

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	@echo "Linking complete!"

# Compile C++ source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)
	@echo "Cleanup complete!"

# Rebuild
rebuild: clean all

# Run the program
run: $(TARGET)
	cd $(BIN_DIR) && ./View.exe

.PHONY: all clean rebuild run