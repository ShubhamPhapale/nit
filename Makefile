# Makefile for VCS - Version Control System

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
LDFLAGS = -lssl -lcrypto -lz

# macOS-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    OPENSSL_PREFIX := $(shell brew --prefix openssl 2>/dev/null || echo /usr/local/opt/openssl)
    CFLAGS += -I$(OPENSSL_PREFIX)/include
    LDFLAGS += -L$(OPENSSL_PREFIX)/lib
endif

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = .

# Files
TARGET = $(BIN_DIR)/nit
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/vcs.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET) vcs *.o
	@echo "Clean complete"

# Install to system
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/$(TARGET)
	@echo "Installed to /usr/local/bin/$(TARGET)"

# Uninstall from system
uninstall:
	rm -f /usr/local/bin/$(TARGET)
	@echo "Uninstalled $(TARGET)"

# Run tests (placeholder)
test: $(TARGET)
	@echo "Running tests..."
	@./test.sh || echo "No tests defined yet"

# Show help
help:
	@echo "VCS Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all (default) - Build the VCS executable"
	@echo "  clean         - Remove build artifacts"
	@echo "  install       - Install VCS to /usr/local/bin"
	@echo "  uninstall     - Remove VCS from /usr/local/bin"
	@echo "  test          - Run tests"
	@echo "  help          - Show this help message"

.PHONY: all clean install uninstall test help
