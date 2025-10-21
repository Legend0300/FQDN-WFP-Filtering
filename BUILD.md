# Build Instructions for FQDN Blocker CLI

## Prerequisites

### Required Software

1. **CMake** (version 3.15 or later)
   - Download from: https://cmake.org/download/
   - During installation, select "Add CMake to system PATH"

2. **Visual Studio 2017 or later**
   - Download Visual Studio Community (free): https://visualstudio.microsoft.com/
   - Required workloads:
     - Desktop development with C++
     - Windows SDK (10.0 or later)

3. **Git** (optional, for version control)
   - Download from: https://git-scm.com/

### Third-Party Dependencies

**nlohmann/json** (Header-only JSON library)
- URL: https://github.com/nlohmann/json
- Download the single header file: https://github.com/nlohmann/json/releases/latest/download/json.hpp
- **IMPORTANT**: Place `json.hpp` in the `lib/` directory before building

## Build Steps

### Step 1: Download nlohmann/json

```powershell
# Navigate to the lib directory
cd lib

# Download json.hpp using PowerShell
Invoke-WebRequest -Uri "https://github.com/nlohmann/json/releases/latest/download/json.hpp" -OutFile "json.hpp"

# Navigate back to project root
cd ..
```

Or manually:
1. Visit: https://github.com/nlohmann/json/releases/latest/download/json.hpp
2. Save as `lib/json.hpp`

### Step 2: Configure with CMake

```powershell
# Create build directory
mkdir build
cd build

# Generate Visual Studio solution (default configuration)
cmake ..

# OR specify Visual Studio version explicitly:
cmake .. -G "Visual Studio 16 2019"
cmake .. -G "Visual Studio 17 2022"
```

### Step 3: Build the Project

**Option A: Using CMake Command Line**

```powershell
# Build Release configuration
cmake --build . --config Release

# Build Debug configuration
cmake --build . --config Debug

# Build with parallel compilation
cmake --build . --config Release -- /m
```

**Option B: Using Visual Studio IDE**

```powershell
# Open the generated solution
start FqdnBlockerCli.sln
```

Then in Visual Studio:
1. Select "Release" or "Debug" configuration
2. Right-click on "FqdnBlockerCli" project
3. Select "Build"

### Step 4: Locate the Executable

After building, the executable will be in:
```
build/Release/FqdnBlockerCli.exe  (Release build)
build/Debug/FqdnBlockerCli.exe    (Debug build)
```

Or directly in `build/` based on CMake configuration.

## Running the Application

### Important: Administrator Privileges Required

This application **must** be run with Administrator privileges to manage Windows Firewall.

**Method 1: Right-Click**
1. Navigate to the executable
2. Right-click `FqdnBlockerCli.exe`
3. Select "Run as administrator"

**Method 2: Elevated PowerShell**
```powershell
# Open PowerShell as Administrator (right-click PowerShell icon → "Run as administrator")
cd path\to\FqdnBlockerCli\build

# Run the application
.\FqdnBlockerCli.exe help
```

**Method 3: Create Shortcut with Elevated Privileges**
1. Right-click `FqdnBlockerCli.exe` → "Create shortcut"
2. Right-click the shortcut → "Properties"
3. Click "Advanced..."
4. Check "Run as administrator"
5. Click OK

## Verification

Test the build with:

```powershell
# Display help (requires admin)
.\FqdnBlockerCli.exe help

# Test with a safe domain
.\FqdnBlockerCli.exe block example.com 60

# List blocks
.\FqdnBlockerCli.exe list

# Remove the test block
.\FqdnBlockerCli.exe remove example.com
```

## Troubleshooting

### CMake Error: "Could not find CMAKE_C_COMPILER"

**Solution**: Install Visual Studio with "Desktop development with C++" workload.

### Error: "json.hpp not found"

**Solution**: 
```powershell
# Ensure json.hpp is in the lib directory
cd lib
Invoke-WebRequest -Uri "https://github.com/nlohmann/json/releases/latest/download/json.hpp" -OutFile "json.hpp"
cd ..

# Clean and rebuild
rm -r build
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Link Error: "unresolved external symbol"

**Solution**: Ensure all Windows libraries are linked. Check CMakeLists.txt includes:
- ws2_32 (Winsock)
- iphlpapi (IP Helper)
- fwpuclnt (Windows Filtering Platform)
- rpcrt4 (RPC Runtime)
- advapi32 (Advanced Windows API)

### "This application requires Administrator privileges"

**Solution**: Always run as Administrator. The application checks privileges on startup.

### Firewall Manager Initialization Failed

**Possible causes**:
1. Not running as Administrator
2. Windows Firewall service disabled

**Solution**:
```powershell
# Check Windows Firewall service status
Get-Service mpssvc

# Start if stopped
Start-Service mpssvc
```

## Clean Build

To perform a clean build:

```powershell
# Remove build directory
rm -r build

# Recreate and build
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Build Configurations

### Debug Build
- Includes debug symbols
- No optimizations
- Larger executable
- Easier to debug

```powershell
cmake --build . --config Debug
```

### Release Build
- Optimized for performance
- Smaller executable
- Recommended for deployment

```powershell
cmake --build . --config Release
```

### RelWithDebInfo Build
- Release optimizations with debug info
- Good for profiling

```powershell
cmake --build . --config RelWithDebInfo
```

## Additional CMake Options

```powershell
# Verbose build output
cmake --build . --config Release --verbose

# Parallel build (faster)
cmake --build . --config Release -j 8

# Install to specific directory (optional)
cmake --install . --prefix "C:\Program Files\FqdnBlockerCli"
```

## Distribution

To distribute the application:

1. Build in Release mode
2. Collect files:
   - `FqdnBlockerCli.exe`
   - `config/config.json`
   - `README.md`
3. Create `data/` and `logs/` directories
4. Package as ZIP or installer

**Note**: Users will need:
- Windows 10 or later
- Administrator privileges
- Windows Firewall enabled

## Development Workflow

### Iterative Development

```powershell
# Make code changes in src/*.cpp or src/*.h

# Rebuild (CMake will detect changes)
cd build
cmake --build . --config Debug

# Test
.\Debug\FqdnBlockerCli.exe help
```

### Adding New Files

If you add new .cpp or .h files:

1. Edit `CMakeLists.txt`
2. Add to `SOURCES` or `HEADERS` section
3. Reconfigure and rebuild:

```powershell
cd build
cmake ..
cmake --build . --config Release
```

## Platform Notes

This application is **Windows-only** and uses:
- Windows Filtering Platform (WFP) APIs
- Winsock for DNS resolution
- Windows-specific security APIs

**Minimum Windows Version**: Windows 10 version 1903 or later (for full WFP dynamic keyword support)

## Next Steps

After successful build:
1. Read the main README.md for usage instructions
2. Test with non-critical domains first
3. Review logs in `logs/fqdn_blocker.log`
4. Check audit store in `data/audit_store.json`

---

**Build Support**: If you encounter issues, please ensure:
- All prerequisites are installed
- json.hpp is in the lib/ directory
- Running CMake from the correct directory
- Using Administrator privileges for execution
