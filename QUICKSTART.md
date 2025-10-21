# Quick Start Guide

## 1. Download nlohmann/json

Before building, you **must** download the JSON library:

### Using PowerShell (Recommended)

```powershell
cd lib
Invoke-WebRequest -Uri "https://github.com/nlohmann/json/releases/latest/download/json.hpp" -OutFile "json.hpp"
cd ..
```

### Manual Download

1. Visit: https://github.com/nlohmann/json/releases/latest/download/json.hpp
2. Save the file as `json.hpp` in the `lib/` folder

## 2. Build the Project

```powershell
# Create and enter build directory
mkdir build
cd build

# Generate project files
cmake ..

# Build Release version
cmake --build . --config Release
```

## 3. Run as Administrator

```powershell
# Right-click FqdnBlockerCli.exe and select "Run as administrator"
# Or from elevated PowerShell:
.\FqdnBlockerCli.exe help
```

## 4. Basic Commands

```powershell
# Block a domain (example)
.\FqdnBlockerCli.exe block example.com

# List all blocks
.\FqdnBlockerCli.exe list

# Remove a block
.\FqdnBlockerCli.exe remove example.com
```

## Common Issues

**"json.hpp not found"**
→ Download json.hpp to the `lib/` directory (see step 1)

**"Administrator privileges required"**
→ Right-click exe → "Run as administrator"

**CMake not found**
→ Install CMake from https://cmake.org/download/

**Compiler not found**
→ Install Visual Studio with "Desktop development with C++"

For detailed instructions, see [BUILD.md](BUILD.md)
