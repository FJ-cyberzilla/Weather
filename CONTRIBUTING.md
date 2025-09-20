# CONTRIBUTING.md
# Contributing to Weather CLI Pro

Thank you for your interest in contributing to Weather CLI Pro! This document provides guidelines and information for contributors.

## 🤝 How to Contribute

### Reporting Bugs
1. Check if the bug has already been reported in [Issues](https://github.com/fj-cyberzilla/weather/issues)
2. Create a new issue with the bug report template
3. Include detailed steps to reproduce the issue
4. Provide system information (OS, compiler version, etc.)

### Suggesting Features
1. Check existing [Issues](https://github.com/fj-cyberzilla/weather/issues) and [Discussions](https://github.com/fj-cyberzilla/weather/discussions)
2. Create a new issue with the feature request template
3. Explain the use case and expected behavior
4. Consider implementation complexity and maintainability

### Code Contributions

#### Development Setup
```bash
git clone https://github.com/fj-cyberzilla/weather
git checkout -b feature/

# Install dependencies (Ubuntu/Debian)
sudo apt-get install libcurl4-openssl-dev nlohmann-json3-dev

# Build and test
make
make test
```

#### Code Style Guidelines
- **C++ Standard**: Use C++17 features
- **Indentation**: 4 spaces, no tabs
- **Naming Conventions**:
  - Classes: `PascalCase` (e.g., `WeatherService`)
  - Functions: `snake_case` (e.g., `get_weather_data`)
  - Variables: `snake_case` (e.g., `api_key`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `API_TIMEOUT`)
- **Headers**: Include guards or `#pragma once`
- **Comments**: Use `//` for single line, `/* */` for blocks
- **Error Handling**: Always handle errors gracefully

#### Pull Request Process
1. Fork the repository
2. Create a feature branch from `develop`
3. Make your changes with tests
4. Update documentation if needed
5. Ensure CI passes
6. Submit a pull request to `develop` branch

#### Testing
- Add unit tests for new functionality
- Ensure existing tests pass
- Test on multiple platforms if possible
- Include integration tests for API interactions

## 📋 Development Guidelines

### Architecture Principles
- **Separation of Concerns**: Keep classes focused on single responsibilities
- **Error Handling**: Use exceptions for exceptional cases, return codes for expected failures
- **Resource Management**: Use RAII principles
- **Thread Safety**: Document thread-safety guarantees

### Adding New Features
1. Create an issue first to discuss the feature
2. Follow the existing code structure
3. Add comprehensive error handling
4. Include user documentation
5. Add appropriate tests

### API Changes
- Maintain backward compatibility when possible
- Document breaking changes in PR description
- Update version numbers according to semantic versioning

## 🧪 Testing

### Running Tests
```bash
make test           # Run all tests
make test-unit      # Run unit tests only
make test-integration # Run integration tests
make coverage       # Generate coverage report
```

### Writing Tests
- Place tests in `tests/` directory
- Use descriptive test names
- Test both success and failure cases
- Mock external dependencies

### Manual Testing
- Test on different operating systems
- Verify color output in different terminals
- Test with various network conditions
- Validate with different API responses

## 📚 Documentation

### Code Documentation
- Document public APIs with clear descriptions
- Include parameter and return value descriptions
- Add usage examples for complex functions
- Keep documentation up-to-date with code changes

### User Documentation
- Update README.md for new features
- Add examples to demonstrate usage
- Update help text and error messages
- Consider creating wiki pages for complex topics

## 🔍 Code Review Process

### What We Look For
- Code quality and maintainability
- Proper error handling
- Security considerations
- Performance implications
- Documentation completeness
- Test coverage

### Review Timeline
- Initial review within 48 hours
- Follow-up reviews within 24 hours
- Merge after approval and CI passing

## 🫆 Debugging

### Common Issues
- **Build Failures**: Check dependency versions
- **Network Issues**: Test with curl directly
- **Color Problems**: Verify terminal capabilities
- **Memory Issues**: Use valgrind for debugging

### Debug Build
```bash
make debug          # Build with debug symbols
gdb ./weather-cli-debug  # Debug with GDB
valgrind ./weather-cli-debug  # Memory debugging
```

## 🌟 Recognition

Contributors will be recognized in:
- README.md contributors section
- Release notes for significant contributions
- GitHub contributors graph
- Special mentions in project updates

## 📞 Getting Help

- **Questions**: Use GitHub Discussions
- **Issues**: Create GitHub Issues
- **Chat**: Join our community channels
- **Email**: Contact maintainers directly

## 📃 Legal

By contributing, you agree that your contributions will be licensed under the MIT License.

---

# .github/ISSUE_TEMPLATE/bug_report.md
---
name: Bug report
about: Create a report to help us improve
title: '[BUG] '
labels: bug
assignees: ''
---

**Describe the bug**
A clear and concise description of what the bug is.

**To Reproduce**
Steps to reproduce the behavior:
1. Run command '...'
2. Enter input '...'
3. See error

**Expected behavior**
A clear and concise description of what you expected to happen.

**Screenshots/Output**
If applicable, add screenshots or paste terminal output to help explain your problem.

**System Information:**
- OS: [e.g. Ubuntu 22.04, macOS 13.0, Windows 11]
- Compiler: [e.g. GCC 11.2, Clang 14.0]
- Terminal: [e.g. GNOME Terminal, iTerm2, Windows Terminal]
- Weather CLI Pro Version: [e.g. 2.0.1]

**Configuration**
- API Key configured: [Yes/No]
- Custom settings: [Any non-default settings]

**Additional context**
Add any other context about the problem here.

---

# .github/ISSUE_TEMPLATE/feature_request.md
---
name: Feature request
about: Suggest an idea for this project
title: '[FEATURE] '
labels: enhancement
assignees: ''
---

**Is your feature request related to a problem? Please describe.**
A clear and concise description of what the problem is. Ex. I'm always frustrated when [...]

**Describe the solution you'd like**
A clear and concise description of what you want to happen.

**Describe alternatives you've considered**
A clear and concise description of any alternative solutions or features you've considered.

**Use Cases**
Describe specific scenarios where this feature would be useful:
- Use case 1: ...
- Use case 2: ...

**Implementation Considerations**
- Impact on existing functionality: [None/Minor/Major]
- Platform compatibility: [All/Linux/macOS/Windows/Termux]
- Performance impact: [None/Low/Medium/High]

**Additional context**
Add any other context or screenshots about the feature request here.

---

# .github/PULL_REQUEST_TEMPLATE.md
## Description
Brief description of changes made in this PR.

## Type of Change
- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Documentation update
- [ ] Performance improvement
- [ ] Code refactoring

## Related Issues
Closes #(issue number)
Fixes #(issue number)
Relates to #(issue number)

## Testing
- [ ] Unit tests pass
- [ ] Integration tests pass
- [ ] Manual testing completed
- [ ] Cross-platform testing (if applicable)

### Test Environment
- OS: [e.g. Ubuntu 22.04]
- Compiler: [e.g. GCC 11.2]
- Dependencies: [Any specific versions]

## Screenshots/Output
If applicable, add screenshots or terminal output showing the changes.

## Checklist
- [ ] My code follows the project's style guidelines
- [ ] I have performed a self-review of my code
- [ ] I have commented my code, particularly in hard-to-understand areas
- [ ] I have made corresponding changes to the documentation
- [ ] My changes generate no new warnings
- [ ] I have added tests that prove my fix is effective or that my feature works
- [ ] New and existing unit tests pass locally with my changes

## Breaking Changes
If this PR introduces breaking changes, describe them here and explain migration steps.

## Performance Impact
Describe any performance implications of these changes.

## Documentation Updates
- [ ] README.md updated
- [ ] Code comments updated
- [ ] API documentation updated (if applicable)

## Additional Notes
Any additional information that reviewers should know about these changes.

---

# Makefile
# Weather CLI Pro Makefile

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
DEBUGFLAGS = -g -DDEBUG -fsanitize=address -fno-omit-frame-pointer
LIBS = -lcurl -lpthread
INCLUDES = -I./include

# Project settings
TARGET = weather-cli
DEBUG_TARGET = weather-cli-debug
SOURCE = weather_cli.cpp
PREFIX = /usr/local

# Build rules
.PHONY: all clean debug test install uninstall help

all: $(TARGET)

$(TARGET): $(SOURCE)
	@echo "🔨 Building Weather CLI Pro..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SOURCE) $(LIBS)
	@echo "✅ Build complete!"

debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(SOURCE)
	@echo "🔧 Building debug version..."
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) $(INCLUDES) -o $(DEBUG_TARGET) $(SOURCE) $(LIBS)
	@echo "✅ Debug build complete!"

test: $(TARGET)
	@echo "🧪 Running tests..."
	@./$(TARGET) --help > /dev/null && echo "✅ Help command works" || echo "❌ Help command failed"
	@echo "🧪 Basic functionality test complete"

clean:
	@echo "🧹 Cleaning build files..."
	rm -f $(TARGET) $(DEBUG_TARGET)
	rm -f *.o *.log
	@echo "✅ Clean complete!"

install: $(TARGET)
	@echo "📦 Installing Weather CLI Pro..."
	install -m 755 $(TARGET) $(PREFIX)/bin/
	@echo "✅ Installation complete! Run with: $(TARGET)"

uninstall:
	@echo "🗑️ Uninstalling Weather CLI Pro..."
	rm -f $(PREFIX)/bin/$(TARGET)
	@echo "✅ Uninstall complete!"

help:
	@echo "Weather CLI Pro - Build System Help"
	@echo "=================================="
	@echo "Available targets:"
	@echo "  all      - Build release version (default)"
	@echo "  debug    - Build debug version with sanitizers"
	@echo "  test     - Run basic functionality tests"
	@echo "  clean    - Remove build artifacts"
	@echo "  install  - Install to system (requires sudo)"
	@echo "  uninstall- Remove from system (requires sudo)"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Example usage:"
	@echo "  make          # Build release version"
	@echo "  make debug    # Build debug version"
	@echo "  make test     # Run tests"
	@echo "  sudo make install  # Install system-wide"

# Dependencies check
check-deps:
	@echo "🔍 Checking dependencies..."
	@pkg-config --exists libcurl && echo "✅ libcurl found" || echo "❌ libcurl not found - install libcurl4-openssl-dev"
	@echo "🔍 Checking for nlohmann/json..."
	@echo '#include <nlohmann/json.hpp>' | $(CXX) -x c++ -c - -o /dev/null 2>/dev/null && echo "✅ nlohmann/json found" || echo "❌ nlohmann/json not found - install nlohmann-json3-dev"

# Static analysis
analyze: $(SOURCE)
	@echo "🔍 Running static analysis..."
	@command -v cppcheck >/dev/null 2>&1 && cppcheck --enable=all --suppress=missingIncludeSystem $(SOURCE) || echo "⚠️ cppcheck not available"
	@command -v clang-tidy >/dev/null 2>&1 && clang-tidy $(SOURCE) -- $(CXXFLAGS) $(INCLUDES) || echo "⚠️ clang-tidy not available"

# Memory check (requires valgrind)
memcheck: $(DEBUG_TARGET)
	@echo "🔍 Running memory check..."
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(DEBUG_TARGET) --help

# Package creation
package: $(TARGET)
	@echo "📦 Creating package..."
	mkdir -p weather-cli-pro-$(shell date +%Y%m%d)
	cp $(TARGET) README.md LICENSE weather-cli-pro-$(shell date +%Y%m%d)/
	tar -czf weather-cli-pro-$(shell date +%Y%m%d).tar.gz weather-cli-pro-$(shell date +%Y%m%d)
	rm -rf weather-cli-pro-$(shell date +%Y%m%d)
	@echo "✅ Package created: weather-cli-pro-$(shell date +%Y%m%d).tar.gz"

---

# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(WeatherCLIPro 
    VERSION 2.0.0
    DESCRIPTION "Professional Weather Intelligence Platform"
    LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(CURL REQUIRED libcurl)

# Find nlohmann/json
find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(${CURL_INCLUDE_DIRS})
include_directories(include/)

# Create executable
add_executable(weather-cli weather_cli.cpp)

# Link libraries
target_link_libraries(weather-cli 
    ${CURL_LIBRARIES}
    nlohmann_json::nlohmann_json
    pthread
)

# Compiler definitions
target_compile_definitions(weather-cli PRIVATE ${CURL_CFLAGS_OTHER})

# Install target
install(TARGETS weather-cli DESTINATION bin)

# CPack configuration
set(CPACK_PACKAGE_NAME "WeatherCLIPro")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "Weather CLI Pro Team")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

include(CPack)

---

# build.sh (Enhanced Termux build script)
#!/bin/bash
# Weather CLI Pro - Enhanced Build Script for Termux
# Supports multiple platforms and dependency management

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Project info
PROJECT_NAME="Weather CLI Pro"
VERSION="2.0.0"
BUILD_DIR="build"
TARGET_NAME="weather-cli"

echo -e "${BLUE}╔══════════════════════════════════════╗${NC}"
echo -e "${BLUE}║${NC}    ${PURPLE}🌤️  Weather CLI Pro Builder${NC}    ${BLUE}║${NC}"
echo -e "${BLUE}║${NC}           ${CYAN}Version ${VERSION}${NC}            ${BLUE}║${NC}"
echo -e "${BLUE}╚══════════════════════════════════════╝${NC}"
echo

# Detect platform
detect_platform() {
    if [[ -n "$TERMUX_VERSION" ]]; then
        PLATFORM="termux"
        PACKAGE_MANAGER="pkg"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        PLATFORM="linux"
        if command -v apt >/dev/null 2>&1; then
            PACKAGE_MANAGER="apt"
        elif command -v yum >/dev/null 2>&1; then
            PACKAGE_MANAGER="yum"
        elif command -v pacman >/dev/null 2>&1; then
            PACKAGE_MANAGER="pacman"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        PLATFORM="macos"
        PACKAGE_MANAGER="brew"
    elif [[ "$OSTYPE" == "msys" ]]; then
        PLATFORM="windows"
        PACKAGE_MANAGER="pacman"
    else
        PLATFORM="unknown"
    fi
    
    echo -e "${CYAN}🔍 Platform detected: ${YELLOW}${PLATFORM}${NC}"
}

# Check dependencies
check_dependency() {
    local cmd=$1
    local package=$2
    
    if command -v "$cmd" >/dev/null 2>&1; then
        echo -e "${GREEN}✅ ${cmd} found${NC}"
        return 0
    else
        echo -e "${RED}❌ ${cmd} not found${NC}"
        if [[ -n "$package" ]]; then
            echo -e "${YELLOW}   📦 Install with: ${PACKAGE_MANAGER} install ${package}${NC}"
        fi
        return 1
    fi
}

# Install dependencies
install_dependencies() {
    echo -e "${BLUE}📦 Checking dependencies...${NC}"
    
    local missing_deps=()
    
    case "$PLATFORM" in
        "termux")
            check_dependency "clang++" "clang" || missing_deps+=("clang")
            check_dependency "curl" "libcurl" || missing_deps+=("libcurl")
            ;;
        "linux")
            check_dependency "g++" "build-essential" || missing_deps+=("build-essential")
            check_dependency "curl-config" "libcurl4-openssl-dev" || missing_deps+=("libcurl4-openssl-dev")
            ;;
        "macos")
            check_dependency "clang++" "xcode-command-line-tools" || missing_deps+=("xcode")
            check_dependency "curl-config" "curl" || missing_deps+=("curl")
            ;;
    esac
    
    # Check for nlohmann/json
    if ! echo '#include <nlohmann/json.hpp>' | ${CXX:-g++} -x c++ -c - -o /dev/null 2>/dev/null; then
        echo -e "${YELLOW}📥 Downloading nlohmann/json...${NC}"
        mkdir -p include/nlohmann
        curl -fsSL https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp \
             -o include/nlohmann/json.hpp
        echo -e "${GREEN}✅ nlohmann/json downloaded${NC}"
    else
        echo -e "${GREEN}✅ nlohmann/json found${NC}"
    fi
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        echo -e "${YELLOW}⚠️  Missing dependencies: ${missing_deps[*]}${NC}"
        read -p "Install missing dependencies? (y/N): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            case "$PLATFORM" in
                "termux")
                    pkg update && pkg install "${missing_deps[@]}"
                    ;;
                "linux")
                    if [[ "$PACKAGE_MANAGER" == "apt" ]]; then
                        sudo apt update && sudo apt install "${missing_deps[@]}"
                    fi
                    ;;
                "macos")
                    brew install "${missing_deps[@]}"
                    ;;
            esac
        fi
    fi
}

# Build function
build_project() {
    echo -e "${BLUE}🔨 Building ${PROJECT_NAME}...${NC}"
    
    # Set compiler based on platform
    if [[ "$PLATFORM" == "termux" ]]; then
        export CXX="clang++"
    else
        export CXX="${CXX:-g++}"
    fi
    
    # Build flags
    CXXFLAGS="-std=c++17 -Wall -Wextra -O2"
    INCLUDES="-I./include"
    LIBS="-lcurl -lpthread"
    
    # Add platform-specific flags
    case "$PLATFORM" in
        "termux")
            CXXFLAGS+=" -static-libgcc -static-libstdc++"
            ;;
        "macos")
            CXXFLAGS+=" -stdlib=libc++"
            ;;
    esac
    
    echo -e "${CYAN}   Compiler: ${CXX}${NC}"
    echo -e "${CYAN}   Flags: ${CXXFLAGS}${NC}"
    
    # Create build directory
    mkdir -p "$BUILD_DIR"
    
    # Compile
    if $CXX $CXXFLAGS $INCLUDES -o "$BUILD_DIR/$TARGET_NAME" weather_cli.cpp $LIBS; then
        echo -e "${GREEN}✅ Build successful!${NC}"
        
        # Make executable
        chmod +x "$BUILD_DIR/$TARGET_NAME"
        
        # Copy to root for convenience
        cp "$BUILD_DIR/$TARGET_NAME" ./"$TARGET_NAME"
        
        return 0
    else
        echo -e "${RED}❌ Build failed!${NC}"
        return 1
    fi
}

# Test function
test_build() {
    echo -e "${BLUE}🧪 Testing build...${NC}"
    
    if ./"$TARGET_NAME" --help >/dev/null 2>&1; then
        echo -e "${GREEN}✅ Basic functionality test passed${NC}"
    else
        echo -e "${YELLOW}⚠️  Help command test failed, but binary might still work${NC}"
    fi
    
    # Test with a quick weather lookup (will fail without API key, but tests networking)
    echo -e "${CYAN}🌐 Testing network connectivity...${NC}"
    if timeout 5s ./"$TARGET_NAME" "London" >/dev/null 2>&1; then
        echo -e "${GREEN}✅ Network test passed${NC}"
    else
        echo -e "${YELLOW}⚠️  Network test failed (expected without API key)${NC}"
    fi
}

# Install function
install_binary() {
    echo
    read -p "📦 Install to system PATH? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        case "$PLATFORM" in
            "termux")
                cp "$TARGET_NAME" "$PREFIX/bin/"
                echo -e "${GREEN}✅ Installed to $PREFIX/bin/${NC}"
                ;;
            "linux"|"macos")
                sudo cp "$TARGET_NAME" "/usr/local/bin/"
                echo -e "${GREEN}✅ Installed to /usr/local/bin/${NC}"
                ;;
        esac
        echo -e "${CYAN}🚀 You can now run: ${TARGET_NAME}${NC}"
    fi
}

# Package function
create_package() {
    echo
    read -p "📦 Create distribution package? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        PACKAGE_NAME="${TARGET_NAME}-${VERSION}-${PLATFORM}"
        mkdir -p "$PACKAGE_NAME"
        
        cp "$TARGET_NAME" "$PACKAGE_NAME/"
        cp README.md "$PACKAGE_NAME/" 2>/dev/null || echo "# Weather CLI Pro" > "$PACKAGE_NAME/README.md"
        cp LICENSE "$PACKAGE_NAME/" 2>/dev/null || echo "MIT License" > "$PACKAGE_NAME/LICENSE"
        
        tar -czf "${PACKAGE_NAME}.tar.gz" "$PACKAGE_NAME"
        rm -rf "$PACKAGE_NAME"
        
        echo -e "${GREEN}✅ Package created: ${PACKAGE_NAME}.tar.gz${NC}"
    fi
}

# Cleanup function
cleanup() {
    if [[ -d "$BUILD_DIR" ]]; then
        echo -e "${YELLOW}🧹 Cleaning build directory...${NC}"
        rm -rf "$BUILD_DIR"
    fi
}

# Main execution
main() {
    detect_platform
    install_dependencies
    
    if build_project; then
        test_build
        install_binary
        create_package
        
        echo
        echo -e "${GREEN}🎉 Build completed successfully!${NC}"
        echo -e "${CYAN}📍 Binary location: ./${TARGET_NAME}${NC}"
        echo -e "${CYAN}🚀 Run with: ./${TARGET_NAME}${NC}"
        echo
        echo -e "${PURPLE}Thank you for using Weather CLI Pro! 🌤️${NC}"
    else
        echo -e "${RED}💥 Build failed. Check the errors above.${NC}"
        exit 1
    fi
}

# Handle script arguments
case "${1:-}" in
    "clean")
        cleanup
        exit 0
        ;;
    "deps")
        detect_platform
        install_dependencies
        exit 0
        ;;
    "test")
        if [[ -f "$TARGET_NAME" ]]; then
            test_build
        else
            echo -e "${RED}❌ Binary not found. Build first.${NC}"
            exit 1
        fi
        exit 0
        ;;
    "help"|"-h"|"--help")
        echo "Weather CLI Pro Build Script"
        echo "Usage: $0 [command]"
        echo
        echo "Commands:"
        echo "  (none)  - Full build process"
        echo "  clean   - Clean build artifacts"
        echo "  deps    - Install dependencies only"
        echo "  test    - Test existing binary"
        echo "  help    - Show this help"
        exit 0
        ;;
    "")
        main
        ;;
    *)
        echo -e "${RED}❌ Unknown command: $1${NC}"
        echo "Use '$0 help' for usage information."
        exit 1
        ;;
esac
