# Makefile for GarageBand VM
# Interprets GBASM assembly and generates audio files

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Targets
TARGET = garagevm
SOURCES = simple_vm.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Main executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Test VM execution
test: $(TARGET)
	@echo "=== Testando VM ==="
	@./$(TARGET) ../out/demo.gbasm -o ../out/demo.wav
	@./$(TARGET) ../out/advanced_test.gbasm -o ../out/advanced_test.wav
	@echo "✓ VM testada com sucesso"

# Clean generated files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Debug build
debug: CXXFLAGS += -DDEBUG -O0 -g
debug: $(TARGET)

# Release build
release: CXXFLAGS += -O3 -DNDEBUG -march=native
release: $(TARGET)

# Help
help:
	@echo "GarageBand VM"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build VM (default)"
	@echo "  test     - Test with examples"
	@echo "  clean    - Remove generated files"
	@echo "  debug    - Build with debug info"
	@echo "  release  - Build optimized version"
	@echo "  help     - Show this help"
	@echo ""
	@echo "Usage:"
	@echo "  ./garagevm input.gbasm -o output.wav"

.PHONY: all test clean debug release help
