# Makefile for GarageBand Compiler
# Compiles BandLang source to GBASM assembly

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
FLEX = flex
BISON = bison

# Targets
TARGET = garagec
SOURCES = main.cpp
GENERATED = lexer.cpp parser.cpp parser.hpp
OBJECTS = $(SOURCES:.cpp=.o) $(GENERATED:.cpp=.o)

# Default target
all: $(TARGET)

# Main executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Flex lexer
lexer.cpp: lexer.l parser.hpp
	$(FLEX) -o $@ $<

# Bison parser
parser.cpp parser.hpp: parser.y
	$(BISON) -d -o parser.cpp $<

# Object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Dependencies
main.o: main.cpp ast.hpp parser.hpp
lexer.o: lexer.cpp parser.hpp ast.hpp
parser.o: parser.cpp ast.hpp

# Test compilation
test: $(TARGET)
	@echo "=== Testando Compilador ==="
	@./$(TARGET) ../examples/demo.band -o ../out/demo.gbasm
	@./$(TARGET) ../examples/advanced_test.band -o ../out/advanced_test.gbasm
	@echo "✓ Compilação testada com sucesso"

# Clean generated files
clean:
	rm -f $(OBJECTS) $(GENERATED) $(TARGET)

# Debug build
debug: CXXFLAGS += -DDEBUG -O0 -g
debug: $(TARGET)

# Help
help:
	@echo "GarageBand Compiler"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build compiler (default)"
	@echo "  test     - Test with examples"
	@echo "  clean    - Remove generated files"
	@echo "  debug    - Build with debug info"
	@echo "  help     - Show this help"
	@echo ""
	@echo "Usage:"
	@echo "  ./garagec input.band -o output.gbasm [--debug]"

.PHONY: all test clean debug help