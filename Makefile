# Root Makefile for GarageBand VM Project
# Coordinates builds for compiler and VM

.PHONY: all compiler vm test clean distclean deps help install examples slides

# Default target
all: compiler vm

# Build compiler
compiler:
	@echo "Building GarageBand Compiler..."
	$(MAKE) -C compiler

# Build VM
vm:
	@echo "Building GarageBand VM..."
	$(MAKE) -C vm

# Run comprehensive tests
test: compiler vm examples
	@echo "Running comprehensive tests..."
	@echo "=========================================="
	@echo "Testing Compiler..."
	$(MAKE) -C compiler test
	@echo "=========================================="
	@echo "Testing VM..."
	$(MAKE) -C vm test
	@echo "=========================================="
	@echo "Running integration tests..."
	@cd tests && ./run_smoke.sh
	@echo "✓ All tests completed"

# Create output directory and compile examples
examples: compiler
	@echo "Compiling example programs..."
	@mkdir -p out
	@for file in examples/*.band; do \
		echo "Compiling $$file..."; \
		./compiler/garagec "$$file" -o "out/$$(basename "$$file" .band).gbasm" || exit 1; \
	done
	@echo "✓ All examples compiled"

# Generate audio from examples
audio: vm examples
	@echo "Generating audio files..."
	@for file in out/*.gbasm; do \
		if [ -f "$$file" ]; then \
			echo "Generating audio for $$file..."; \
			./vm/garagevm "$$file" -o "out/$$(basename "$$file" .gbasm).wav" || exit 1; \
		fi; \
	done
	@echo "✓ All audio files generated"
	@ls -la out/*.wav 2>/dev/null || echo "No WAV files found"

# Generate slides/presentation
slides:
	@echo "Generating slides..."
	$(MAKE) -C slides
	@echo "✓ Slides generated: slides/APS_GarageBand.pdf"

# Install to system (optional)
install: all
	@echo "Installing GarageBand tools..."
	@mkdir -p ~/.local/bin
	@cp compiler/garagec ~/.local/bin/
	@cp vm/garagevm ~/.local/bin/
	@echo "✓ Installed to ~/.local/bin/"
	@echo "Add ~/.local/bin to your PATH if not already present"

# Clean object files
clean:
	@echo "Cleaning build artifacts..."
	$(MAKE) -C compiler clean
	$(MAKE) -C vm clean
	@echo "✓ Build artifacts cleaned"

# Clean everything including outputs
distclean:
	@echo "Deep cleaning all generated files..."
	$(MAKE) -C compiler distclean
	$(MAKE) -C vm distclean
	rm -rf out/*.gbasm out/*.wav out/*.mp3
	@echo "✓ All generated files removed"

# Install dependencies
deps:
	@echo "Installing dependencies for all components..."
	$(MAKE) -C compiler deps
	$(MAKE) -C vm deps
	@echo "Installing additional tools..."
	@sudo apt install -y tree file hexdump || echo "Some tools may not be available"

# Check project status
status:
	@echo "GarageBand VM Project Status"
	@echo "=========================================="
	@echo "Compiler:"
	@if [ -f compiler/garagec ]; then \
		echo "  ✓ Built ($(shell ls -la compiler/garagec | awk '{print $$5}') bytes)"; \
	else \
		echo "  ✗ Not built"; \
	fi
	@echo "VM:"
	@if [ -f vm/garagevm ]; then \
		echo "  ✓ Built ($(shell ls -la vm/garagevm | awk '{print $$5}') bytes)"; \
	else \
		echo "  ✗ Not built"; \
	fi
	@echo "Examples:"
	@for file in examples/*.band; do \
		gbasm="out/$$(basename "$$file" .band).gbasm"; \
		wav="out/$$(basename "$$file" .band).wav"; \
		echo -n "  $$(basename "$$file"): "; \
		if [ -f "$$gbasm" ]; then echo -n "gbasm ✓ "; else echo -n "gbasm ✗ "; fi; \
		if [ -f "$$wav" ]; then echo "wav ✓"; else echo "wav ✗"; fi; \
	done
	@echo "Dependencies:"
	@echo -n "  flex: "; which flex > /dev/null && flex --version | head -1 | awk '{print $$2}' || echo "✗ Missing"
	@echo -n "  bison: "; which bison > /dev/null && bison --version | head -1 | awk '{print $$4}' || echo "✗ Missing"
	@echo -n "  g++: "; which g++ > /dev/null && g++ --version | head -1 | awk '{print $$3}' || echo "✗ Missing"
	@echo -n "  MP3 support: "; pkg-config --exists lame && echo "✓ Available" || echo "✗ Missing (optional)"

# Development workflow
dev: all examples audio
	@echo "Development build complete!"
	@echo "Files generated:"
	@ls -la out/ 2>/dev/null || echo "No output files"

# CI/CD simulation
ci: deps all test
	@echo "CI/CD pipeline completed successfully!"

# Performance benchmark
benchmark: all examples
	@echo "Running performance benchmarks..."
	$(MAKE) -C vm benchmark
	@echo "Compiler performance:"
	@time $(MAKE) examples > /dev/null 2>&1
	@echo "VM performance:"
	@time $(MAKE) audio > /dev/null 2>&1

# Validate project structure
validate:
	@echo "Validating project structure..."
	@for dir in docs compiler vm examples tests out slides; do \
		if [ -d "$$dir" ]; then \
			echo "  ✓ $$dir/"; \
		else \
			echo "  ✗ $$dir/ (missing)"; \
		fi; \
	done
	@for file in README.md docs/EBNF.md docs/GBASM.md; do \
		if [ -f "$$file" ]; then \
			echo "  ✓ $$file"; \
		else \
			echo "  ✗ $$file (missing)"; \
		fi; \
	done

# Help
help:
	@echo "GarageBand VM Project - Root Makefile"
	@echo "======================================"
	@echo ""
	@echo "Build targets:"
	@echo "  all        - Build compiler and VM (default)"
	@echo "  compiler   - Build only the compiler"
	@echo "  vm         - Build only the VM"
	@echo ""
	@echo "Test targets:"
	@echo "  test       - Run comprehensive tests"
	@echo "  examples   - Compile example .band files to .gbasm"
	@echo "  audio      - Generate .wav files from .gbasm"
	@echo ""
	@echo "Maintenance targets:"
	@echo "  clean      - Remove build artifacts"
	@echo "  distclean  - Remove all generated files"
	@echo "  deps       - Install dependencies"
	@echo ""
	@echo "Utility targets:"
	@echo "  status     - Show project build status"
	@echo "  validate   - Check project structure"
	@echo "  benchmark  - Run performance tests"
	@echo "  dev        - Full development build"
	@echo "  ci         - Simulate CI/CD pipeline"
	@echo "  install    - Install tools to ~/.local/bin"
	@echo "  slides     - Generate presentation"
	@echo ""
	@echo "Quick start:"
	@echo "  make deps     # Install dependencies"
	@echo "  make all      # Build everything"
	@echo "  make test     # Run tests"
	@echo "  make dev      # Full development workflow"
