# Logger Library

A high-performance asynchronous logging library implemented in C++20 with thread-safe operations and a lock-free queue.

## Features

- **Asynchronous Logging**: Non-blocking log operations using background thread processing
- **Thread-Safe**: Multi-threaded logging with unique thread identification
- **Lock-Free Queue**: High-performance logging using Boost lockfree queue
- **Multiple Log Levels**: INFO, WARNING, ERROR with configurable filtering
- **File Rotation**: Automatic log file management with timestamp-based naming
- **Colored Console Output**: Enhanced readability with color-coded log levels
- **C++20 Formatting**: Modern std::format integration for efficient string formatting
- **Modular Build System**: Clean CMake structure with separate builds for tests, benchmarks, and examples
- **Unit Tests**: Comprehensive GoogleTest suite for validation
- **Benchmarks**: Performance testing with Google Benchmark

## Project Structure

```
logger/
├── build.sh              # Convenient build script
├── CMakeLists.txt        # Root build configuration
├── Include/              # Library headers and source
│   ├── Logger.h
│   ├── Logger.cpp
│   ├── Formatter.h
│   ├── LogData.h
│   ├── LogData.cpp
│   ├── LogLevel.h
│   └── Macros.h
├── Example/              # Basic usage example
│   ├── CMakeLists.txt
│   └── Example.cpp
└── Tests/                # All test-related code
    ├── CMakeLists.txt
    ├── UnitTests/        # GoogleTest unit tests
    │   ├── CMakeLists.txt
    │   └── test.cpp
    ├── Benchmarks/       # Performance benchmarks
    │   ├── CMakeLists.txt
    │   └── Benchmarks.cpp
    └── StressTest/       # Multi-threaded stress test
        ├── CMakeLists.txt
        └── Main.cpp
```

## Quick Start

```bash
# 1. Install dependencies (macOS)
brew install gcc boost cmake

# 2. Clone and navigate to the project
cd logger/

# 3. Build and test
./build.sh all

# 4. Run the example
./build/Example/logger_example
```

That's it! See the sections below for detailed installation instructions for other platforms and advanced usage.

## Requirements

### System Dependencies

- **C++20 Compiler**: GCC-15 or later (recommended via Homebrew on macOS)
- **Boost Libraries**: Version 1.89.0 or later with lockfree module
- **CMake**: Version 3.10 or later
- **pthread**: Threading support

### Installing Dependencies (macOS with Homebrew)

```bash
# Install GCC-15 for C++20 support
brew install gcc

# Install Boost libraries
brew install boost

# Install CMake (if not already installed)
brew install cmake
```

### Installing Dependencies (Ubuntu/Debian)

```bash
# Install GCC and development tools
sudo apt update
sudo apt install gcc-13 g++-13 cmake build-essential

# Install Boost libraries
sudo apt install libboost-all-dev

# Set GCC-13 as default (optional)
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100
```

### Installing Dependencies (CentOS/RHEL/Fedora)

```bash
# For RHEL/CentOS 8+
sudo dnf install gcc-toolset-13-gcc gcc-toolset-13-gcc-c++ cmake make
sudo dnf install boost-devel

# For Fedora
sudo dnf install gcc gcc-c++ cmake make boost-devel

# Enable GCC toolset (RHEL/CentOS)
source /opt/rh/gcc-toolset-13/enable
```

### Installing Dependencies (Arch Linux)

```bash
# Install GCC and development tools
sudo pacman -S gcc cmake make

# Install Boost libraries
sudo pacman -S boost
```

### Installing Dependencies (Windows)

#### Option 1: Using vcpkg (Recommended)

```powershell
# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install Boost
.\vcpkg install boost:x64-windows

# Install CMake (if not already installed)
# Download from https://cmake.org/download/
```

#### Option 2: Using MSYS2/MinGW

```bash
# Install MSYS2 from https://www.msys2.org/
# Open MSYS2 UCRT64 terminal

# Update package database
pacman -Syu

# Install GCC and development tools
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-make

# Install Boost
pacman -S mingw-w64-ucrt-x86_64-boost

# Add to PATH: C:\msys64\ucrt64\bin
```

#### Option 3: Using Visual Studio

```powershell
# Install Visual Studio 2022 with C++ development tools
# Ensure C++20 support is installed

# Install Boost via vcpkg (as shown in Option 1)
# Or download pre-compiled Boost binaries from boost.org
```

## Building the Project

This project uses a modular CMake structure with a convenient build script for easy compilation.

### Quick Build with build.sh (Recommended)

The easiest way to build is using the provided `build.sh` script:

```bash
# Make the script executable (first time only)
chmod +x build.sh

# Build only the library
./build.sh lib

# Build and run unit tests
./build.sh unitTests

# Build benchmarks (requires GCC-15 on macOS)
./build.sh benchmarks

# Build example executable
./build.sh example

# Build stress test
./build.sh stressTest

# Build everything (all targets + tests)
./build.sh all

# Clean build directory
./build.sh clean

# Build in debug mode
./build.sh unitTests --debug

# Show help
./build.sh --help
```

**Features of build.sh:**
- Automatically configures CMake with appropriate flags for each target
- Builds only what you need (faster builds)
- Runs tests automatically for `unitTests` and `all` targets
- Shows colored output and executable locations
- Uses all available CPU cores for faster compilation
- Cross-platform compatible (macOS, Linux)

### Manual CMake Build (Alternative)

If you prefer manual control or are on Windows:

#### Linux/macOS
```bash
# Navigate to the logger directory
cd logger/

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
make

# Or build specific targets
make logger          # Library only
make logger_example  # Example application
make logger_main     # Stress test application
```

#### Windows (Command Prompt)
```cmd
REM Navigate to the logger directory
cd logger

REM Create build directory
mkdir build
cd build

REM Configure for Visual Studio (default generator)
cmake ..

REM Build with MSBuild
cmake --build . --config Release

REM Or build specific targets
cmake --build . --target logger --config Release
cmake --build . --target logger_example --config Release
cmake --build . --target logger_main --config Release
```

#### Windows (MSYS2/MinGW)
```bash
# Navigate to the logger directory (in MSYS2 UCRT64 terminal)
cd logger/

# Create build directory
mkdir build
cd build

# Configure with MinGW Makefiles
cmake .. -G "MinGW Makefiles"
mingw32-make

# Or build specific targets
mingw32-make logger
mingw32-make logger_example
mingw32-make logger_main
```

### Direct Compilation (Alternative)

#### macOS (with Homebrew)
```bash
# Navigate to Example directory
cd logger/Example/

# Compile with GCC-15
/opt/homebrew/bin/gcc-15 -std=c++20 -I../Include -I/opt/homebrew/include \
    Example.cpp ../Include/Logger.cpp ../Include/LogData.cpp \
    -lstdc++ -lpthread -o Example

# Run the example
./Example
```

#### Linux (Ubuntu/Debian)
```bash
# Navigate to Example directory
cd logger/Example/

# Compile with GCC-13
g++-13 -std=c++20 -I../Include \
    Example.cpp ../Include/Logger.cpp ../Include/LogData.cpp \
    -lboost_system -lpthread -o Example

# Run the example
./Example
```

#### Windows (MSYS2/MinGW)
```bash
# Navigate to Example directory (in MSYS2 UCRT64 terminal)
cd logger/Example/

# Compile with MinGW GCC
g++ -std=c++20 -I../Include \
    Example.cpp ../Include/Logger.cpp ../Include/LogData.cpp \
    -lboost_system -lpthread -o Example.exe

# Run the example
./Example.exe
```

#### Windows (Visual Studio Command Prompt)
```cmd
REM Navigate to Example directory
cd logger\Example\

REM Compile with MSVC (requires vcpkg Boost)
cl /std:c++20 /I..\Include /I[vcpkg_root]\installed\x64-windows\include ^
   Example.cpp ..\Include\Logger.cpp ..\Include\LogData.cpp ^
   /link /LIBPATH:[vcpkg_root]\installed\x64-windows\lib

REM Run the example
Example.exe
```

## Running the Applications

### 1. Basic Example (`logger_example`)

Demonstrates basic logging functionality with different log levels:

```bash
cd build/
./logger_example
```

**Expected Output:**
```
[13.24.25.640621000-17.11.25][INFO][TRACE][void exampleFunction() ][12241308112218997552]| Program started!
[13.24.25.640681000-17.11.25][WARNING][DEBUG][void exampleFunction() ][12241308112218997552]| Potential issue detected
[13.24.25.640716000-17.11.25][ERROR][PROD][void exampleFunction() ][12241308112218997552]| Critical failure!
```

### 2. Stress Test (`logger_main`)

Multi-threaded stress test demonstrating concurrent logging:

```bash
cd build/
./logger_main
```

**Features Demonstrated:**
- Multiple worker threads logging simultaneously
- Thread-safe operations with unique thread IDs
- High-volume concurrent log generation
- Lock-free queue performance under load

### 3. Unit Tests (`logger_tests`)

**Enable and Build Tests:**

```bash
# Clean build with tests enabled
cd logger/
rm -rf build && mkdir build && cd build

# Configure with tests
cmake .. -DENABLE_TESTS=ON

# Build
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # macOS
```

**Run Tests:**

```bash
# Run all tests
./logger_tests

# Or use CTest
ctest

# Run with verbose output
./logger_tests --gtest_verbose
```

**Expected Output:**
```
Running main() from googletest
[==========] Running 4 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 4 tests from TestLogger
[ RUN      ] TestLogger.Singleton
[       OK ] TestLogger.Singleton (0 ms)
[ RUN      ] TestLogger.SetLevel
[       OK ] TestLogger.SetLevel (0 ms)
[ RUN      ] TestLogger.BlockLog
[       OK ] TestLogger.BlockLog (0 ms)
[ RUN      ] TestLogger.ToString
[       OK ] TestLogger.ToString (0 ms)
[----------] 4 tests from TestLogger (0 ms total)

[----------] Global test environment tear-down
[==========] 4 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 4 tests.
```

### 4. Performance Benchmarks (`logger_benchmark`)

**Important Note:** Benchmarks require **GCC-15** on macOS due to Google Benchmark regex backend issues with Apple Clang. The CMakeLists.txt automatically uses GCC-15 if available via Homebrew.

**Enable and Build Benchmarks:**

```bash
# Ensure GCC-15 is installed (macOS)
brew install gcc@15

# Clean build with benchmarks enabled
cd logger/
rm -rf build && mkdir build && cd build

# Configure with benchmarks
cmake .. -DENABLE_BENCHMARKS=ON

# Build
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # macOS
```

**Run Benchmarks:**

```bash
# Run all benchmarks
./logger_benchmark

# Run with specific parameters
./logger_benchmark --benchmark_repetitions=3
./logger_benchmark --benchmark_filter=BM_2Threads

# Save results to file
./logger_benchmark --benchmark_out=results.json --benchmark_out_format=json
```

**Example Output:**
```
2025-11-17T14:47:02+01:00
Running ./logger_benchmark
Run on (10 X 24 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB
  L1 Instruction 128 KiB
  L2 Unified 4096 KiB (x10)
----------------------------------------------------------------------
Benchmark                            Time             CPU   Iterations
----------------------------------------------------------------------
BM_2Threads/iterations:1000       2957 us         32.2 us         1000
BM_4Threads/iterations:1000       2910 us         60.2 us         1000
BM_6Threads/iterations:1000       3219 us         87.9 us         1000
```

### 5. Build All Components

**Build everything (core + tests + benchmarks):**

```bash
cd logger/
rm -rf build && mkdir build && cd build

# Configure with all optional components
cmake .. -DENABLE_TESTS=ON -DENABLE_BENCHMARKS=ON

# Build all targets
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # macOS
```

**Available Targets:**
- `logger` - Static library
- `logger_example` - Basic example
- `logger_main` - Stress test
- `logger_tests` - Unit tests (if enabled)
- `logger_benchmark` - Performance benchmarks (if enabled)

## Configuration Options

### Log Types

- `INFO`: General information messages
- `WARNING`: Warning messages for potential issues
- `ERROR`: Error messages for critical failures

### Log Levels

- `LogLevel::Trace`: Development/debugging environment
- `LogLevel::Debug`: Testing environment
- `LogLevel::Prod`: Production environment

### File Output

Logs are automatically written to timestamped files in the format:
```
DD-MM-YY-HH_MM_SS.txt
```

## Usage Example

```cpp
#include "Logger.h"

int main() {
    // Initialize logger with INFO level, file output enabled
    auto& logger = Logger::getInstance(
        LogLevel::Prod,     // Minimum log level
        "./Logs/",          // Log directory
        100,                // Max log files
        true,               // Enable file output
        "MyApp"             // Application prefix
    );

    // Log messages
    LOG_INFO("Application started");
    LOG_WARNING("Configuration file not found, using defaults");
    LOG_ERROR("Database connection failed");

    // Shutdown logger (flushes remaining logs)
    logger.shutDown();
    return 0;
}
```

## Troubleshooting

### Common Issues

1. **Compilation Errors with std::format**
    - Ensure you're using C++20 standard
    - Use GCC-15 or later, MSVC 2022, or Clang 15+ for full C++20 support

2. **Boost Headers Not Found**
    - **macOS**: `brew list boost`
    - **Ubuntu/Debian**: `dpkg -l | grep boost`
    - **CentOS/RHEL**: `rpm -qa | grep boost`
    - **Windows**: Check vcpkg installation or MSYS2 packages
    - Verify include path in CMakeLists.txt

3. **Threading Issues**
    - **Linux/macOS**: Ensure pthread library is linked: `-lpthread`
    - **Windows**: Use `/MT` or `/MD` runtime with MSVC
    - Verify thread support in compiler

4. **CMake Configuration Issues**
    - **Linux/macOS**: Clear build directory: `rm -rf build/`
    - **Windows**: Clear build directory: `rmdir /s build`
    - Reconfigure: `mkdir build && cd build && cmake ..`

5. **Platform-Specific Issues**
    - **Windows**: Ensure proper runtime library linking (`/MT`, `/MD`)
    - **Linux**: May need to install `libc++-dev` for some distributions
    - **macOS**: Use Homebrew packages to avoid system library conflicts

6. **Benchmark Compilation Issues (macOS)**
    - **Problem**: `Failed to determine the source files for the regular expression backend`
    - **Cause**: Apple Clang has issues with Google Benchmark's regex detection
    - **Solution**:
      ```bash
      # Install GCC-15 via Homebrew
      brew install gcc@15
      
      # The CMakeLists.txt automatically uses GCC-15 if available
      # Clean rebuild
      rm -rf build && mkdir build && cd build
      cmake .. -DENABLE_BENCHMARKS=ON
      make
      ```
    - **Alternative**: Use `logger_main` stress test for performance evaluation

7. **Tests Not Building**
    - Ensure tests are enabled: `cmake .. -DENABLE_TESTS=ON`
    - GoogleTest is automatically downloaded via FetchContent
    - If download fails, check internet connection or use system-installed GoogleTest

### Performance Tips

- Use appropriate log levels to filter unnecessary messages
- Consider file output impact on performance for high-frequency logging
- Monitor queue depth in high-volume scenarios
- Properly shutdown logger to ensure all messages are flushed

## Quick Reference

### Build Commands Summary

```bash
# Basic build (core targets only)
cmake .. && make

# With tests
cmake .. -DENABLE_TESTS=ON && make

# With benchmarks (requires GCC-15 on macOS)
cmake .. -DENABLE_BENCHMARKS=ON && make

# Everything
cmake .. -DENABLE_TESTS=ON -DENABLE_BENCHMARKS=ON && make
```

### Run Commands Summary

```bash
# Core executables (always available)
./logger_example     # Basic demonstration
./logger_main        # Multi-threaded stress test

# Optional components (if built)
./logger_tests       # Run unit tests
./logger_benchmark   # Run performance benchmarks
ctest                # Run tests via CTest
```

## Architecture

The logger uses a producer-consumer pattern:
- **Producer**: Application threads add log entries to lock-free queue
- **Consumer**: Background logging thread processes queue and writes to outputs
- **Thread Safety**: Achieved through lock-free data structures and atomic operations
- **Formatting**: Efficient C++20 std::format for string composition

## License

[Add your license information here]
