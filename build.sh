#!/bin/bash

# Weather CLI Pro - Build Script
# A comprehensive build script for multiple platforms

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
PROJECT_NAME="Weather CLI Pro"
VERSION="2.0.0"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
INSTALL_PREFIX="/usr/local"
TARGET="weather-cli"

# Platform detection
detect_platform() {
    case "$(uname -s)" in
        Linux*)
            if [ -n "$TERMUX_VERSION" ]; then
                echo "termux"
            else
                echo "linux"
            fi
            ;;
        Darwin*) echo "macos" ;;
        MINGW*|MSYS*|CYGWIN*) echo "windows" ;;
        *) echo "unknown" ;;
    esac
}

PLATFORM=$(detect_platform)

# Function to print colored output
print_status() {
    echo -e "${BLUE}==>${NC} ${1}"
}

print_success() {
    echo -e "${GREEN}✅${NC} ${1}"
}

print_warning() {
    echo -e "${YELLOW}⚠️${NC} ${1}"
}

print_error() {
    echo -e "${RED}❌${NC} ${1}"
}

# Function to check dependencies
check_dependencies() {
    print_status "Checking dependencies for ${PLATFORM}..."
    
    # Check for C++ compiler
    if command -v g++ >/dev/null 2>&1; then
        CXX="g++"
    elif command -v clang++ >/dev/null 2>&1; then
        CXX="clang++"
    else
        print_error "No C++ compiler found. Please install g++ or clang++"
        exit 1
    fi
    print_success "Compiler found: $($CXX --version | head -n1)"
    
    # Check for curl development files
    if [ "$PLATFORM" = "termux" ]; then
        if pkg list-installed | grep -q "libcurl" || pkg list-installed | grep -q "curl"; then
            print_success "libcurl found"
        else
            print_warning "libcurl not found"
        fi
    else
        if pkg-config --exists libcurl 2>/dev/null; then
            print_success "libcurl found"
        elif [ -f "/usr/include/curl/curl.h" ] || [ -f "/usr/local/include/curl/curl.h" ]; then
            print_success "libcurl headers found"
        else
            print_warning "libcurl development files not found"
        fi
    fi
    
    # Check for nlohmann/json
    if [ -f "/usr/include/nlohmann/json.hpp" ] || \
       [ -f "/usr/local/include/nlohmann/json.hpp" ] || \
       [ -f "${SCRIPT_DIR}/include/nlohmann/json.hpp" ]; then
        print_success "nlohmann/json found"
    else
        print_warning "nlohmann/json not found"
    fi
    
    # Check for make
    if command -v make >/dev/null 2>&1; then
        print_success "make found"
    else
        print_error "make not found. Please install make"
        exit 1
    fi
}

# Function to install dependencies
install_dependencies() {
    print_status "Installing dependencies for ${PLATFORM}..."
    
    case "$PLATFORM" in
        linux)
            if command -v apt-get >/dev/null 2>&1; then
                sudo apt-get update
                sudo apt-get install -y build-essential libcurl4-openssl-dev nlohmann-json3-dev
            elif command -v yum >/dev/null 2>&1; then
                sudo yum install -y gcc-c++ make libcurl-devel
            elif command -v dnf >/dev/null 2>&1; then
                sudo dnf install -y gcc-c++ make libcurl-devel
            elif command -v zypper >/dev/null 2>&1; then
                sudo zypper install -y gcc-c++ make libcurl-devel
            else
                print_error "Unsupported Linux package manager"
                exit 1
            fi
            ;;
        macos)
            if command -v brew >/dev/null 2>&1; then
                brew update
                brew install curl nlohmann-json
            else
                print_error "Homebrew not found. Please install Homebrew first"
                exit 1
            fi
            ;;
        termux)
            pkg update
            pkg install -y clang make libcurl nlohmann-json
            ;;
        windows)
            print_warning "Please install dependencies manually on Windows:"
            echo "1. Install MSYS2: https://www.msys2.org/"
            echo "2. Run: pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-curl mingw-w64-x86_64-nlohmann-json"
            ;;
        *)
            print_error "Unsupported platform: $PLATFORM"
            exit 1
            ;;
    esac
}

# Function to download nlohmann/json
download_json() {
    print_status "Downloading nlohmann/json header..."
    mkdir -p "${SCRIPT_DIR}/include/nlohmann"
    if curl -fsSL https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp \
        -o "${SCRIPT_DIR}/include/nlohmann/json.hpp"; then
        print_success "nlohmann/json downloaded successfully"
    else
        print_error "Failed to download nlohmann/json"
        exit 1
    fi
}

# Function to build the project
build_project() {
    local build_type="$1"
    local extra_flags="$2"
    
    print_status "Building ${PROJECT_NAME} (${build_type}) for ${PLATFORM}..."
    
    case "$build_type" in
        release)
            make release $extra_flags
            ;;
        debug)
            make debug $extra_flags
            ;;
        profile)
            make profile $extra_flags
            ;;
        all)
            make all-builds $extra_flags
            ;;
        *)
            print_error "Unknown build type: $build_type"
            exit 1
            ;;
    esac
    
    if [ $? -eq 0 ]; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        exit 1
    fi
}

# Function to run tests
run_tests() {
    print_status "Running tests..."
    make test
    if [ $? -eq 0 ]; then
        print_success "Tests passed"
    else
        print_error "Tests failed"
        exit 1
    fi
}

# Function to install the application
install_app() {
    print_status "Installing ${PROJECT_NAME}..."
    
    if [ "$PLATFORM" = "termux" ]; then
        make install PREFIX="$PREFIX"
    else
        if [ "$(id -u)" -eq 0 ]; then
            make install
        else
            sudo make install
        fi
    fi
    
    if [ $? -eq 0 ]; then
        print_success "Installation completed successfully"
    else
        print_error "Installation failed"
        exit 1
    fi
}

# Function to create a package
create_package() {
    print_status "Creating package..."
    make package
    if [ $? -eq 0 ]; then
        print_success "Package created successfully"
    else
        print_error "Package creation failed"
        exit 1
    fi
}

# Function to clean build artifacts
clean_build() {
    print_status "Cleaning build artifacts..."
    make clean
    print_success "Clean completed"
}

# Function to show usage
show_usage() {
    echo -e "${CYAN}${PROJECT_NAME} Build Script v${VERSION}${NC}"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -d, --deps          Install dependencies"
    echo "  -c, --check         Check dependencies only"
    echo "  -b, --build TYPE    Build project (release, debug, profile, all)"
    echo "  -t, --test          Run tests after build"
    echo "  -i, --install       Install after build"
    echo "  -p, --package       Create package after build"
    echo "  -f, --force         Force rebuild"
    echo "  -j, --json          Download nlohmann/json header"
    echo "  --clean             Clean build artifacts"
    echo "  --prefix DIR        Set installation prefix (default: /usr/local)"
    echo ""
    echo "Examples:"
    echo "  $0 --deps --build release --install  # Full install"
    echo "  $0 --build debug --test              # Debug build with tests"
    echo "  $0 --check                           # Check dependencies only"
    echo "  $0 --clean                           # Clean build artifacts"
}

# Main function
main() {
    local action="build"
    local build_type="release"
    local install_app=false
    local create_package=false
    local run_tests=false
    local check_deps=false
    local install_deps=false
    local download_json=false
    local clean=false
    local force=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -h|--help)
                show_usage
                exit 0
                ;;
            -d|--deps)
                install_deps=true
                shift
                ;;
            -c|--check)
                check_deps=true
                shift
                ;;
            -b|--build)
                build_type="$2"
                shift 2
                ;;
            -t|--test)
                run_tests=true
                shift
                ;;
            -i|--install)
                install_app=true
                shift
                ;;
            -p|--package)
                create_package=true
                shift
                ;;
            -j|--json)
                download_json=true
                shift
                ;;
            --clean)
                clean=true
                shift
                ;;
            -f|--force)
                force=true
                shift
                ;;
            --prefix)
                INSTALL_PREFIX="$2"
                shift 2
                ;;
            *)
                print_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    # Export variables for make
    export INSTALL_PREFIX
    
    # Show banner
    echo -e "${CYAN}"
    echo "╔══════════════════════════════════════════════════════════════╗"
    echo "║                 Weather CLI Pro Build System                 ║"
    echo "║                        Version ${VERSION}                        ║"
    echo "╚══════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
    echo "Platform: $PLATFORM"
    echo "Build directory: $BUILD_DIR"
    echo ""
    
    # Check dependencies
    check_dependencies
    
    # Install dependencies if requested
    if [ "$install_deps" = true ]; then
        install_dependencies
    fi
    
    # Download nlohmann/json if requested
    if [ "$download_json" = true ]; then
        download_json
    fi
    
    # Clean if requested
    if [ "$clean" = true ]; then
        clean_build
    fi
    
    # Check dependencies only
    if [ "$check_deps" = true ]; then
        exit 0
    fi
    
    # Build the project
    local make_flags=""
    if [ "$force" = true ]; then
        make_flags="--always-make"
    fi
    
    build_project "$build_type" "$make_flags"
    
    # Run tests if requested
    if [ "$run_tests" = true ]; then
        run_tests
    fi
    
    # Install if requested
    if [ "$install_app" = true ]; then
        install_app
    fi
    
    # Create package if requested
    if [ "$create_package" = true ]; then
        create_package
    fi
    
    print_success "${PROJECT_NAME} build process completed successfully!"
    echo ""
    echo -e "${GREEN}🚀 You can now run: ./${TARGET}${NC}"
}

# Run main function with all arguments
main "$@"
