#!/bin/bash

# Frikadellen BAF Build Script
# This script helps build the project with different configurations

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
INSTALL_PREFIX="${HOME}/.local"
BUILD_TYPE="Release"
WITH_BOTCRAFT="ON"
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Functions
print_header() {
    echo -e "${BLUE}════════════════════════════════════════${NC}"
    echo -e "${BLUE}  $1${NC}"
    echo -e "${BLUE}════════════════════════════════════════${NC}"
}

print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --demo)
            WITH_BOTCRAFT="OFF"
            shift
            ;;
        --clean)
            print_info "Cleaning build directory..."
            rm -rf "$BUILD_DIR"
            shift
            ;;
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  --debug      Build in Debug mode (default: Release)"
            echo "  --demo       Build without Botcraft (demonstration mode)"
            echo "  --clean      Clean build directory before building"
            echo "  --prefix DIR Set installation prefix (default: ~/.local)"
            echo "  --help       Show this help message"
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Print configuration
print_header "Build Configuration"
echo "Build Type:       $BUILD_TYPE"
echo "With Botcraft:    $WITH_BOTCRAFT"
echo "Build Directory:  $BUILD_DIR"
echo "Install Prefix:   $INSTALL_PREFIX"
echo "Parallel Jobs:    $JOBS"
echo ""

# Check for required tools
print_header "Checking Prerequisites"

command -v cmake >/dev/null 2>&1 || {
    print_error "CMake is required but not installed."
    echo "Install with: sudo apt-get install cmake (Ubuntu/Debian)"
    echo "           or: brew install cmake (macOS)"
    exit 1
}
print_info "CMake found: $(cmake --version | head -n1)"

command -v g++ >/dev/null 2>&1 || command -v clang++ >/dev/null 2>&1 || {
    print_error "C++ compiler is required but not installed."
    echo "Install with: sudo apt-get install build-essential (Ubuntu/Debian)"
    echo "           or: xcode-select --install (macOS)"
    exit 1
}
if command -v g++ >/dev/null 2>&1; then
    print_info "GCC found: $(g++ --version | head -n1)"
elif command -v clang++ >/dev/null 2>&1; then
    print_info "Clang found: $(clang++ --version | head -n1)"
fi

# Check for Botcraft if needed
if [ "$WITH_BOTCRAFT" = "ON" ]; then
    if ! pkg-config --exists botcraft 2>/dev/null && ! [ -f "/usr/local/lib/cmake/botcraft/botcraftConfig.cmake" ]; then
        print_warn "Botcraft not found in standard locations"
        print_warn "The build might fail if Botcraft is not installed"
        print_warn "Use --demo flag to build without Botcraft"
        echo ""
        read -p "Continue anyway? (y/N) " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            exit 1
        fi
    else
        print_info "Botcraft found"
    fi
fi

# Create build directory
print_header "Configuring Project"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Run CMake
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DBUILD_WITH_BOTCRAFT="$WITH_BOTCRAFT" \
      -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
      .. || {
    print_error "CMake configuration failed"
    exit 1
}

print_info "Configuration successful"
echo ""

# Build
print_header "Building Project"
cmake --build . --config "$BUILD_TYPE" -j "$JOBS" || {
    print_error "Build failed"
    exit 1
}

print_info "Build successful"
echo ""

# Print results
print_header "Build Complete"
print_info "Executable: $BUILD_DIR/frikadellen-baf"
echo ""
echo "To install:"
echo "  cd $BUILD_DIR && sudo cmake --install ."
echo ""
echo "To run:"
echo "  ./$BUILD_DIR/frikadellen-baf"
echo ""
echo "To configure:"
echo "  cp config.toml.example config.toml"
echo "  nano config.toml"
echo ""

# Offer to run
if [ "$BUILD_TYPE" = "Debug" ] || [ "$WITH_BOTCRAFT" = "OFF" ]; then
    read -p "Run now? (y/N) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        # Copy config if it doesn't exist
        if [ ! -f "config.toml" ]; then
            cp ../config.toml.example config.toml
            print_info "Created config.toml from template"
        fi
        ./frikadellen-baf
    fi
fi
