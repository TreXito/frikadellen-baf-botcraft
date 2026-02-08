# Build Instructions

This document provides detailed instructions for building Frikadellen BAF for Minecraft 1.21.11.

## Prerequisites

### Required

- **CMake** 3.15 or higher
- **C++17 compatible compiler**:
  - GCC 7+ (Linux)
  - Clang 5+ (macOS)
  - MSVC 2017+ (Windows)
- **Git**

### Optional (for full functionality)

- **Botcraft** - Minecraft bot framework for version 1.21.11
- **nlohmann_json** - JSON parsing library  
- **websocketpp** - WebSocket client library
- **OpenSSL** - For HTTPS/WSS connections
- **libcurl** - For webhook HTTP requests

## Building Botcraft (Required for Full Functionality)

### Linux/macOS

```bash
# Install dependencies
# Ubuntu/Debian:
sudo apt-get install build-essential cmake git libssl-dev zlib1g-dev

# Fedora:
sudo dnf install cmake gcc-c++ git openssl-devel zlib-devel

# macOS:
brew install cmake openssl zlib

# Clone Botcraft
git clone https://github.com/adepierre/Botcraft.git
cd Botcraft

# Configure for Minecraft 1.21.11
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release \
      -DBOTCRAFT_GAME_VERSION=1.21.1 \
      -DBOTCRAFT_BUILD_EXAMPLES=OFF \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      ..

# Build and install
cmake --build . --config Release -j$(nproc)
sudo cmake --install .
```

### Windows (Visual Studio)

```powershell
# Clone Botcraft
git clone https://github.com/adepierre/Botcraft.git
cd Botcraft

# Configure
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ^
      -DBOTCRAFT_GAME_VERSION=1.21.1 ^
      -DBOTCRAFT_BUILD_EXAMPLES=OFF ^
      ..

# Build
cmake --build . --config Release

# Install (run as Administrator)
cmake --install . --config Release
```

## Building Frikadellen BAF

### Option 1: Full Build (with Botcraft)

```bash
# Clone repository
git clone https://github.com/TreXito/frikadellen-baf-botcraft.git
cd frikadellen-baf-botcraft

# Create build directory
mkdir build && cd build

# Configure
cmake -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_WITH_BOTCRAFT=ON \
      ..

# Build
cmake --build . --config Release

# The executable will be at: build/frikadellen-baf
```

### Option 2: Demo Build (without Botcraft)

If you want to build and test the code structure without full Botcraft integration:

```bash
# Clone repository
git clone https://github.com/TreXito/frikadellen-baf-botcraft.git
cd frikadellen-baf-botcraft

# Create build directory
mkdir build && cd build

# Configure without Botcraft
cmake -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_WITH_BOTCRAFT=OFF \
      ..

# Build
cmake --build . --config Release
```

**Note**: Demo build will compile but won't actually connect to Minecraft. It's useful for:
- Testing configuration system
- Verifying code structure
- Development without Hypixel access

## Installing Dependencies

### nlohmann_json

#### Ubuntu/Debian
```bash
sudo apt-get install nlohmann-json3-dev
```

#### Fedora
```bash
sudo dnf install json-devel
```

#### macOS
```bash
brew install nlohmann-json
```

#### From source
```bash
git clone https://github.com/nlohmann/json.git
cd json
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
sudo cmake --install .
```

### websocketpp (Optional - for production WebSocket)

```bash
git clone https://github.com/zaphoyd/websocketpp.git
cd websocketpp
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
sudo cmake --install .
```

### libcurl (Optional - for webhooks)

#### Ubuntu/Debian
```bash
sudo apt-get install libcurl4-openssl-dev
```

#### Fedora
```bash
sudo dnf install libcurl-devel
```

#### macOS
```bash
brew install curl
```

## Build Options

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_WITH_BOTCRAFT` | `ON` | Enable Botcraft integration |
| `BOTCRAFT_USE_SYSTEM` | `OFF` | Use system-installed Botcraft |
| `CMAKE_BUILD_TYPE` | - | `Release` or `Debug` |
| `CMAKE_INSTALL_PREFIX` | `/usr/local` | Installation directory |

### Examples

**Debug build**:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

**Custom install location**:
```bash
cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local ..
```

**Use system Botcraft**:
```bash
cmake -DBOTCRAFT_USE_SYSTEM=ON ..
```

## Troubleshooting

### Botcraft not found

```
CMake Error: Could not find package botcraft
```

**Solution**: Install Botcraft first (see "Building Botcraft" section above)

### C++17 not supported

```
CMake Error: C++17 standard not supported
```

**Solution**: Update your compiler:
- GCC: `sudo apt-get install g++-7` or higher
- Clang: `sudo apt-get install clang-5` or higher
- MSVC: Update Visual Studio to 2017 or later

### nlohmann_json not found

```
Warning: nlohmann_json not found
```

**Solution**: Install nlohmann_json (see "Installing Dependencies" section)

### OpenSSL errors

```
Could not find OpenSSL
```

**Solution**:
- Linux: `sudo apt-get install libssl-dev`
- macOS: `brew install openssl`
- Windows: Use vcpkg: `vcpkg install openssl`

## Cross-Compilation

### For Windows on Linux

```bash
# Install MinGW
sudo apt-get install mingw-w64

# Configure
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/mingw-toolchain.cmake \
      -DCMAKE_BUILD_TYPE=Release \
      ..
```

### For Linux ARM

```bash
# Install ARM toolchain
sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

# Configure
cmake -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
      -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
      -DCMAKE_BUILD_TYPE=Release \
      ..
```

## Static Linking

For a portable binary without dependencies:

```bash
cmake -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_SHARED_LIBS=OFF \
      -DCMAKE_EXE_LINKER_FLAGS="-static" \
      ..
```

**Note**: Static linking may not work on all platforms, especially macOS.

## Verification

After building, verify the executable:

```bash
# Check binary
file build/frikadellen-baf

# List dependencies (Linux)
ldd build/frikadellen-baf

# List dependencies (macOS)
otool -L build/frikadellen-baf

# Test configuration loading
./build/frikadellen-baf --version
```

## Next Steps

After successful build:

1. Copy `config.toml.example` to `config.toml`
2. Edit `config.toml` with your settings
3. Run the bot: `./frikadellen-baf`
4. See [README.md](README.md) for usage instructions

## Getting Help

If you encounter issues:

1. Check the [GitHub Issues](https://github.com/TreXito/frikadellen-baf-botcraft/issues)
2. Join the [Discord server](https://discord.gg/eYRBsaECzY)
3. Read the [Botcraft documentation](https://adepierre.github.io/Botcraft/)
