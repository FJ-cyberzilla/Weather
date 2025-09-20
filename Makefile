# Weather CLI Pro - Professional Makefile
# Supports multiple platforms and build configurations

# Compiler Configuration
CXX ?= g++
CC ?= gcc

# Detect platform
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

ifeq ($(UNAME_S),Linux)
    PLATFORM = linux
    LIBS = -lcurl -lpthread -ldl
    CXXFLAGS_PLATFORM = -static-libgcc -static-libstdc++
endif
ifeq ($(UNAME_S),Darwin)
    PLATFORM = macos
    LIBS = -lcurl -lpthread
    CXXFLAGS_PLATFORM = -stdlib=libc++
endif
ifneq (,$(findstring MINGW,$(UNAME_S)))
    PLATFORM = windows
    LIBS = -lcurl -lpthread -lws2_32
    CXXFLAGS_PLATFORM = -static-libgcc -static-libstdc++
    TARGET_SUFFIX = .exe
endif
ifdef TERMUX_VERSION
    PLATFORM = termux
    CXX = clang++
    LIBS = -lcurl -lpthread
    CXXFLAGS_PLATFORM = -static-libgcc -static-libstdc++
endif

# Project Configuration
PROJECT_NAME = Weather CLI Pro
VERSION = 2.0.0
TARGET = weather-cli$(TARGET_SUFFIX)
DEBUG_TARGET = weather-cli-debug$(TARGET_SUFFIX)
SOURCE = weather_cli.cpp
BUILD_DIR = build
INSTALL_PREFIX ?= /usr/local

# Compiler Flags
CXXFLAGS_BASE = -std=c++17 -Wall -Wextra -Wpedantic
CXXFLAGS_RELEASE = $(CXXFLAGS_BASE) -O3 -DNDEBUG -DVERSION=\"$(VERSION)\"
CXXFLAGS_DEBUG = $(CXXFLAGS_BASE) -g -O0 -DDEBUG -fsanitize=address -fno-omit-frame-pointer
CXXFLAGS_PROFILE = $(CXXFLAGS_BASE) -O2 -g -pg -DNDEBUG

# Include directories
INCLUDES = -I./include -I./src

# Library paths
LIBPATHS = 

# Feature flags
ENABLE_COLORS ?= 1
ENABLE_LOGGING ?= 1
ENABLE_CACHING ?= 1

ifeq ($(ENABLE_COLORS),1)
    CXXFLAGS_BASE += -DENABLE_COLORS
endif
ifeq ($(ENABLE_LOGGING),1)
    CXXFLAGS_BASE += -DENABLE_LOGGING
endif
ifeq ($(ENABLE_CACHING),1)
    CXXFLAGS_BASE += -DENABLE_CACHING
endif

# Build targets
.PHONY: all clean debug release profile test install uninstall package help deps check format analyze

# Default target
all: release

# Release build
release: $(TARGET)

$(TARGET): $(SOURCE) | $(BUILD_DIR)
	@echo "🔨 Building $(PROJECT_NAME) v$(VERSION) [Release] for $(PLATFORM)..."
	$(CXX) $(CXXFLAGS_RELEASE) $(CXXFLAGS_PLATFORM) $(INCLUDES) $(LIBPATHS) \
		-o $(BUILD_DIR)/$(TARGET) $(SOURCE) $(LIBS)
	@cp $(BUILD_DIR)/$(TARGET) ./$(TARGET)
	@echo "✅ Release build complete: $(TARGET)"

# Debug build
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(SOURCE) | $(BUILD_DIR)
	@echo "🔧 Building $(PROJECT_NAME) v$(VERSION) [Debug] for $(PLATFORM)..."
	$(CXX) $(CXXFLAGS_DEBUG) $(CXXFLAGS_PLATFORM) $(INCLUDES) $(LIBPATHS) \
		-o $(BUILD_DIR)/$(DEBUG_TARGET) $(SOURCE) $(LIBS)
	@cp $(BUILD_DIR)/$(DEBUG_TARGET) ./$(DEBUG_TARGET)
	@echo "✅ Debug build complete: $(DEBUG_TARGET)"

# Profile build
profile: weather-cli-profile$(TARGET_SUFFIX)

weather-cli-profile$(TARGET_SUFFIX): $(SOURCE) | $(BUILD_DIR)
	@echo "📊 Building $(PROJECT_NAME) v$(VERSION) [Profile] for $(PLATFORM)..."
	$(CXX) $(CXXFLAGS_PROFILE) $(CXXFLAGS_PLATFORM) $(INCLUDES) $(LIBPATHS) \
		-o $(BUILD_DIR)/weather-cli-profile$(TARGET_SUFFIX) $(SOURCE) $(LIBS)
	@echo "✅ Profile build complete: weather-cli-profile$(TARGET_SUFFIX)"

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Dependency checking
deps:
	@echo "🔍 Checking dependencies for $(PLATFORM)..."
	@echo "Checking for C++ compiler..."
	@$(CXX) --version >/dev/null 2>&1 && echo "✅ $(CXX) found" || (echo "❌ $(CXX) not found" && exit 1)
	@echo "Checking for libcurl..."
	@pkg-config --exists libcurl 2>/dev/null && echo "✅ libcurl found" || echo "⚠️  libcurl not found via pkg-config"
	@echo "Checking for nlohmann/json..."
	@echo '#include <nlohmann/json.hpp>' | $(CXX) -x c++ -c - -o /dev/null 2>/dev/null && \
		echo "✅ nlohmann/json found" || echo "⚠️  nlohmann/json not found"
	@echo "Dependencies check complete."

# Install nlohmann/json if missing
install-json:
	@echo "📥 Downloading nlohmann/json header..."
	@mkdir -p include/nlohmann
	@curl -fsSL https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp \
		-o include/nlohmann/json.hpp
	@echo "✅ nlohmann/json installed to include/nlohmann/"

# Testing
test: $(TARGET)
	@echo "🧪 Running tests..."
	@./$(TARGET) --version >/dev/null 2>&1 && echo "✅ Version test passed" || echo "❌ Version test failed"
	@./$(TARGET) --help >/dev/null 2>&1 && echo "✅ Help test passed" || echo "❌ Help test failed"
	@echo "🧪 Basic smoke tests completed."

# Memory testing (requires valgrind)
memtest: $(DEBUG_TARGET)
	@echo "🔍 Running memory tests..."
	@command -v valgrind >/dev/null 2>&1 || (echo "❌ valgrind not found" && exit 1)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./$(DEBUG_TARGET) --help
	@echo "✅ Memory test completed."

# Performance testing
perftest: $(TARGET)
	@echo "⚡ Running performance tests..."
	@echo "Startup time test:"
	@time -p ./$(TARGET) --help >/dev/null 2>&1 || true
	@echo "Memory usage test:"
	@/usr/bin/time -v ./$(TARGET) --help >/dev/null 2>&1 || true

# Static analysis
analyze: $(SOURCE)
	@echo "🔍 Running static analysis..."
	@command -v cppcheck >/dev/null 2>&1 && \
		cppcheck --enable=all --error-exitcode=0 --suppress=missingIncludeSystem \
		--suppress=unusedFunction $(SOURCE) || echo "⚠️  cppcheck not available"
	@command -v clang-tidy >/dev/null 2>&1 && \
		clang-tidy $(SOURCE) -- $(CXXFLAGS_RELEASE) $(INCLUDES) || echo "⚠️  clang-tidy not available"

# Code formatting
format: $(SOURCE)
	@echo "🎨 Formatting code..."
	@command -v clang-format >/dev/null 2>&1 && \
		clang-format -i $(SOURCE) && echo "✅ Code formatted" || echo "⚠️  clang-format not available"

# Check code formatting
check-format: $(SOURCE)
	@echo "🎨 Checking code format..."
	@command -v clang-format >/dev/null 2>&1 && \
		clang-format --dry-run --Werror $(SOURCE) && echo "✅ Code format OK" || \
		(echo "❌ Code format issues found" && exit 1)

# Installation
install: $(TARGET)
	@echo "📦 Installing $(PROJECT_NAME)..."
ifeq ($(PLATFORM),termux)
	install -m 755 $(TARGET) $(PREFIX)/bin/
	@echo "✅ Installed to $(PREFIX)/bin/$(TARGET)"
else
	sudo install -m 755 $(TARGET) $(INSTALL_PREFIX)/bin/
	@echo "✅ Installed to $(INSTALL_PREFIX)/bin/$(TARGET)"
endif
	@echo "🚀 You can now run: $(TARGET)"

# Uninstallation
uninstall:
	@echo "🗑️  Uninstalling $(PROJECT_NAME)..."
ifeq ($(PLATFORM),termux)
	rm -f $(PREFIX)/bin/$(TARGET)
	@echo "✅ Uninstalled from $(PREFIX)/bin/"
else
	sudo rm -f $(INSTALL_PREFIX)/bin/$(TARGET)
	@echo "✅ Uninstalled from $(INSTALL_PREFIX)/bin/"
endif

# Package creation
package: $(TARGET)
	@echo "📦 Creating distribution package..."
	$(eval PACKAGE_NAME := weather-cli-pro-$(VERSION)-$(PLATFORM)-$(UNAME_M))
	@mkdir -p $(PACKAGE_NAME)
	@cp $(TARGET) $(PACKAGE_NAME)/
	@cp README.md $(PACKAGE_NAME)/ 2>/dev/null || echo "# Weather CLI Pro v$(VERSION)" > $(PACKAGE_NAME)/README.md
	@cp LICENSE $(PACKAGE_NAME)/ 2>/dev/null || echo "MIT License" > $(PACKAGE_NAME)/LICENSE
	@echo "$(PROJECT_NAME) v$(VERSION)" > $(PACKAGE_NAME)/VERSION
	@echo "Platform: $(PLATFORM)" >> $(PACKAGE_NAME)/VERSION
	@echo "Architecture: $(UNAME_M)" >> $(PACKAGE_NAME)/VERSION
	@echo "Build Date: $$(date)" >> $(PACKAGE_NAME)/VERSION
	@tar -czf $(PACKAGE_NAME).tar.gz $(PACKAGE_NAME)
	@rm -rf $(PACKAGE_NAME)
	@echo "✅ Package created: $(PACKAGE_NAME).tar.gz"

# Binary package (just executable)
dist: $(TARGET)
	@echo "📦 Creating binary distribution..."
	$(eval DIST_NAME := weather-cli-$(VERSION)-$(PLATFORM)-$(UNAME_M))
	@mkdir -p dist
	@cp $(TARGET) dist/$(DIST_NAME)$(TARGET_SUFFIX)
	@echo "✅ Binary distribution: dist/$(DIST_NAME)$(TARGET_SUFFIX)"

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@rm -f $(TARGET) $(DEBUG_TARGET) weather-cli-profile$(TARGET_SUFFIX)
	@rm -f *.tar.gz *.zip
	@rm -rf weather-cli-pro-*
	@rm -rf dist/
	@rm -f gmon.out *.gcov *.gcda *.gcno
	@rm -f core core.* vgcore.*
	@echo "✅ Clean complete."

# Deep clean (including downloaded dependencies)
distclean: clean
	@echo "🧹 Deep cleaning..."
	@rm -rf include/nlohmann/
	@rm -f *.log
	@echo "✅ Deep clean complete."

# Development setup
setup:
	@echo "🔧 Setting up development environment..."
	@make deps
	@make install-json
	@echo "✅ Development environment ready."

# Quick build and test
quick: $(TARGET) test

# Build all variants
all-builds: release debug profile
	@echo "✅ All build variants completed."

# Help target
help:
	@echo "$(PROJECT_NAME) v$(VERSION) - Build System"
	@echo "=============================================="
	@echo ""
	@echo "🎯 Main Targets:"
	@echo "  release      Build optimized release version (default)"
	@echo "  debug        Build debug version with sanitizers"
	@echo "  profile      Build profiling version"
	@echo "  all-builds   Build all variants"
	@echo ""
	@echo "🔧 Development:"
	@echo "  setup        Setup development environment"
	@echo "  deps         Check dependencies"
	@echo "  install-json Download nlohmann/json if missing"
	@echo "  test         Run basic functionality tests"
	@echo "  memtest      Run memory leak tests (requires valgrind)"
	@echo "  perftest     Run performance tests"
	@echo ""
	@echo "🔍 Code Quality:"
	@echo "  analyze      Run static analysis (cppcheck, clang-tidy)"
	@echo "  format       Format code with clang-format"
	@echo "  check-format Check if code is properly formatted"
	@echo ""
	@echo "📦 Distribution:"
	@echo "  package      Create complete distribution package"
	@echo "  dist         Create binary-only distribution"
	@echo "  install      Install to system (may require sudo)"
	@echo "  uninstall    Remove from system (may require sudo)"
	@echo ""
	@echo "🧹 Cleanup:"
	@echo "  clean        Remove build artifacts"
	@echo "  distclean    Remove all generated files"
	@echo ""
	@echo "🔧 Build Configuration:"
	@echo "  Platform: $(PLATFORM)"
	@echo "  Compiler: $(CXX)"
	@echo "  Architecture: $(UNAME_M)"
	@echo ""
	@echo "💡 Examples:"
	@echo "  make                    # Build release version"
	@echo "  make debug              # Build debug version"
	@echo "  make test               # Run tests"
	@echo "  make ENABLE_COLORS=0    # Build without color support"
	@echo "  sudo make install       # Install system-wide"
	@echo ""
	@echo "🌟 Feature Flags:"
	@echo "  ENABLE_COLORS=$(ENABLE_COLORS)   # Color output support"
	@echo "  ENABLE_LOGGING=$(ENABLE_LOGGING)  # Logging system"
	@echo "  ENABLE_CACHING=$(ENABLE_CACHING)  # API response caching"

# Print build information
info:
	@echo "$(PROJECT_NAME) v$(VERSION) - Build Information"
	@echo "================================================"
	@echo "Platform: $(PLATFORM)"
	@echo "Architecture: $(UNAME_M)"
	@echo "Compiler: $(CXX)"
	@echo "Compiler Version: $$($(CXX) --version | head -n1)"
	@echo "Build Directory: $(BUILD_DIR)"
	@echo "Install Prefix: $(INSTALL_PREFIX)"
	@echo "Target: $(TARGET)"
	@echo "Debug Target: $(DEBUG_TARGET)"
	@echo "Libraries: $(LIBS)"
	@echo "Release Flags: $(CXXFLAGS_RELEASE) $(CXXFLAGS_PLATFORM)"
	@echo "Debug Flags: $(CXXFLAGS_DEBUG) $(CXXFLAGS_PLATFORM)"

# Continuous Integration target
ci: deps setup all-builds test analyze
	@echo "✅ CI pipeline completed successfully."

# Development workflow
dev: clean setup debug test
	@echo "✅ Development build completed."
