#!/bin/bash

# Logger Build Script
# Usage: ./build.sh [target] [options]
# Examples:
#   ./build.sh lib
#   ./build.sh unitTests
#   ./build.sh benchmarks
#   ./build.sh example
#   ./build.sh stressTest
#   ./build.sh all
#   ./build.sh clean

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Build directory
BUILD_DIR="build"
CMAKE_ARGS=""
BUILD_TARGET=""

# Print usage
print_usage() {
    echo -e "${BLUE}Logger Build Script${NC}"
    echo ""
    echo "Usage: ./build.sh [target] [options]"
    echo ""
    echo "Targets:"
    echo "  lib          - Build only the logger library"
    echo "  example      - Build the example executable"
    echo "  stressTest   - Build the stress test executable"
    echo "  unitTests    - Build and run unit tests"
    echo "  benchmarks   - Build benchmarks (requires GCC-15 on macOS)"
    echo "  all          - Build everything (tests + benchmarks + examples)"
    echo "  clean        - Remove build directory"
    echo ""
    echo "Options:"
    echo "  --debug      - Build with debug symbols (default: Release)"
    echo "  --help       - Show this help message"
    echo ""
    echo "Examples:"
    echo "  ./build.sh lib"
    echo "  ./build.sh unitTests"
    echo "  ./build.sh benchmarks --debug"
    echo "  ./build.sh all"
    echo ""
}

# Parse arguments
TARGET=""
BUILD_TYPE="Release"

while [[ $# -gt 0 ]]; do
    case $1 in
        lib|example|stressTest|unitTests|benchmarks|all|clean)
            TARGET="$1"
            shift
            ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --help|-h)
            print_usage
            exit 0
            ;;
        *)
            echo -e "${RED}Error: Unknown option '$1'${NC}"
            print_usage
            exit 1
            ;;
    esac
done

# If no target specified, show usage
if [ -z "$TARGET" ]; then
    print_usage
    exit 1
fi

# Handle clean target
if [ "$TARGET" = "clean" ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
    echo -e "${GREEN}✓ Clean complete${NC}"
    exit 0
fi

# Create build directory
mkdir -p "$BUILD_DIR"

# Configure CMake based on target
echo -e "${BLUE}Building target: ${TARGET}${NC}"
echo -e "${BLUE}Build type: ${BUILD_TYPE}${NC}"
echo ""

case $TARGET in
    lib)
        CMAKE_ARGS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DENABLE_TESTS=OFF -DENABLE_BENCHMARKS=OFF -DENABLE_EXAMPLE=OFF -DENABLE_MAIN=OFF"
        BUILD_TARGET="logger"
        ;;
    example)
        CMAKE_ARGS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DENABLE_TESTS=OFF -DENABLE_BENCHMARKS=OFF -DENABLE_EXAMPLE=ON -DENABLE_MAIN=OFF"
        BUILD_TARGET="logger_example"
        ;;
    stressTest)
        CMAKE_ARGS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DENABLE_TESTS=OFF -DENABLE_BENCHMARKS=OFF -DENABLE_EXAMPLE=OFF -DENABLE_MAIN=ON"
        BUILD_TARGET="logger_main"
        ;;
    unitTests)
        CMAKE_ARGS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DENABLE_TESTS=ON -DENABLE_BENCHMARKS=OFF -DENABLE_EXAMPLE=OFF -DENABLE_MAIN=OFF"
        BUILD_TARGET="logger_tests"
        ;;
    benchmarks)
        CMAKE_ARGS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DENABLE_TESTS=OFF -DENABLE_BENCHMARKS=ON -DENABLE_EXAMPLE=OFF -DENABLE_MAIN=OFF"
        BUILD_TARGET="logger_benchmark"
        ;;
    all)
        CMAKE_ARGS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DENABLE_TESTS=ON -DENABLE_BENCHMARKS=ON -DENABLE_EXAMPLE=ON -DENABLE_MAIN=ON"
        BUILD_TARGET="all"
        ;;
esac

# Configure and build
echo -e "${YELLOW}Configuring CMake...${NC}"
cd "$BUILD_DIR"
cmake .. $CMAKE_ARGS

echo ""
echo -e "${YELLOW}Building...${NC}"
cmake --build . --target $BUILD_TARGET -j$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

echo ""
echo -e "${GREEN}✓ Build complete${NC}"
echo ""

# Run tests if applicable
if [ "$TARGET" = "unitTests" ]; then
    echo -e "${YELLOW}Running unit tests...${NC}"
    ctest --output-on-failure
    echo ""
    echo -e "${GREEN}✓ Tests complete${NC}"
elif [ "$TARGET" = "all" ]; then
    echo -e "${YELLOW}Running unit tests...${NC}"
    ctest --output-on-failure
    echo ""
    echo -e "${GREEN}✓ All targets built and tested${NC}"
fi

# Print executable locations
echo ""
echo -e "${BLUE}Output files:${NC}"
case $TARGET in
    lib)
        echo -e "  Library: ${GREEN}${BUILD_DIR}/liblogger.a${NC}"
        ;;
    example)
        echo -e "  Executable: ${GREEN}${BUILD_DIR}/Example/logger_example${NC}"
        echo -e "  Run with: ${YELLOW}${BUILD_DIR}/Example/logger_example${NC}"
        ;;
    stressTest)
        echo -e "  Executable: ${GREEN}${BUILD_DIR}/Tests/StressTest/logger_main${NC}"
        echo -e "  Run with: ${YELLOW}${BUILD_DIR}/Tests/StressTest/logger_main${NC}"
        ;;
    unitTests)
        echo -e "  Executable: ${GREEN}${BUILD_DIR}/Tests/UnitTests/logger_tests${NC}"
        echo -e "  Run with: ${YELLOW}${BUILD_DIR}/Tests/UnitTests/logger_tests${NC}"
        ;;
    benchmarks)
        echo -e "  Executable: ${GREEN}${BUILD_DIR}/Tests/Benchmarks/logger_benchmark${NC}"
        echo -e "  Run with: ${YELLOW}${BUILD_DIR}/Tests/Benchmarks/logger_benchmark${NC}"
        ;;
    all)
        echo -e "  Library: ${GREEN}${BUILD_DIR}/liblogger.a${NC}"
        echo -e "  Example: ${GREEN}${BUILD_DIR}/Example/logger_example${NC}"
        echo -e "  Stress Test: ${GREEN}${BUILD_DIR}/Tests/StressTest/logger_main${NC}"
        echo -e "  Unit Tests: ${GREEN}${BUILD_DIR}/Tests/UnitTests/logger_tests${NC}"
        echo -e "  Benchmarks: ${GREEN}${BUILD_DIR}/Tests/Benchmarks/logger_benchmark${NC}"
        ;;
esac
echo ""
