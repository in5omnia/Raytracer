# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Source files (all cpp files in current directory)
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

# Target executable
TARGET = raytracer

# Main target
all: $(TARGET)

# Link object files
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: all

# Run with a specific scene file
test: $(TARGET)
	./$(TARGET) test_scene.json

.PHONY: all clean run debug test