# FQDN Blocker CLI

A Windows command-line application for blocking Fully Qualified Domain Names (FQDNs) using Windows Filtering Platform (WFP) with dynamic DNS resolution and automatic firewall rule updates.

## Features

- **Block FQDNs by Domain Name**: Block websites and services by their domain names instead of IP addresses
- **Dynamic DNS Resolution**: Automatically resolves FQDNs to IP addresses (IPv4 and IPv6)
- **Automatic IP Updates**: Periodically refreshes DNS resolutions and updates firewall rules
- **Persistent Storage**: Maintains audit log of all blocked domains with JSON-based persistence
- **Scheduled Refresh**: Background scheduler automatically updates IP addresses at configurable intervals
- **Boot Pre-hydration**: On application start, refreshes all existing blocks to ensure current IP addresses
- **Windows Firewall Integration**: Uses Windows Filtering Platform APIs for robust firewall management

## Requirements

- **Operating System**: Windows 10 or later
- **Privileges**: Administrator rights (required for firewall management)
- **Build Tools**: 
  - CMake 3.15 or later
  - Visual Studio 2017 or later (with C++17 support)
  - Windows SDK

## Project Structure

```
FqdnBlockerCli/
├── src/                    # Source files
│   ├── main.cpp           # CLI entry point and command handling
│   ├── Config.h/cpp       # Configuration management
│   ├── AuditLogger.h/cpp  # Audit logging and persistence
│   ├── FirewallManager.h/cpp  # Windows Firewall Platform wrapper
│   ├── Resolver.h/cpp     # DNS resolution utilities
│   └── Scheduler.h/cpp    # Background task scheduling
├── include/               # Additional headers
├── lib/                   # Third-party libraries (json.hpp)
├── config/                # Configuration files
│   └── config.json       # Default configuration
├── data/                  # Audit store (generated at runtime)
├── logs/                  # Log files (generated at runtime)
├── build/                 # Build output
└── CMakeLists.txt        # CMake build configuration
```

## Installation & Building

### Step 1: Clone or Download

Download this project to your local machine.

### Step 2: Install nlohmann/json

Download the single-header JSON library:

1. Go to: https://github.com/nlohmann/json/releases/latest/download/json.hpp
2. Download `json.hpp`
3. Place it in the `lib/` directory

### Step 3: Build with CMake

```powershell
# Navigate to project directory
cd FqdnBlockerCli

# Create build directory
mkdir build
cd build

# Generate Visual Studio solution
cmake ..

# Build the project
cmake --build . --config Release
```

Alternatively, open the generated `.sln` file in Visual Studio and build from there.

### Step 4: Run as Administrator

The built executable requires Administrator privileges:

```powershell
# Right-click and select "Run as Administrator"
# Or from an elevated PowerShell:
.\build\FqdnBlockerCli.exe help
```

## Usage

### Commands

#### Block an FQDN

Block a domain name with optional refresh interval:

```powershell
FqdnBlockerCli.exe block <fqdn> [interval_minutes]
```

**Examples:**
```powershell
# Block example.com with default refresh interval
FqdnBlockerCli.exe block example.com

# Block example.com with 120-minute refresh interval
FqdnBlockerCli.exe block example.com 120
```

#### List Blocked FQDNs

Display all currently blocked domains:

```powershell
FqdnBlockerCli.exe list
```

#### Refresh All Blocks

Manually trigger DNS resolution refresh for all blocked domains:

```powershell
FqdnBlockerCli.exe refresh
```

#### Remove a Block

Remove a blocked domain and its firewall rules:

```powershell
FqdnBlockerCli.exe remove <fqdn>
```

**Example:**
```powershell
FqdnBlockerCli.exe remove example.com
```

#### Set Default Interval

Configure the default refresh interval (in minutes):

```powershell
FqdnBlockerCli.exe set-interval <minutes>
```

**Example:**
```powershell
FqdnBlockerCli.exe set-interval 120
```

#### Help

Display usage information:

```powershell
FqdnBlockerCli.exe help
```

## Configuration

The application uses a JSON configuration file located at `config/config.json`:

```json
{
  "defaultInterval": 60,
  "logFilePath": "logs/fqdn_blocker.log",
  "auditStorePath": "data/audit_store.json"
}
```

**Configuration Options:**
- `defaultInterval`: Default refresh interval in minutes (default: 60)
- `logFilePath`: Path to the log file
- `auditStorePath`: Path to the audit store (JSON database)

## How It Works

### Algorithm Overview

1. **Initialization**:
   - Load configuration from `config/config.json`
   - Initialize audit logger, firewall manager, resolver, and scheduler
   - Perform boot pre-hydration (refresh existing blocks)

2. **Block Command**:
   - Resolve FQDN to IP addresses (IPv4/IPv6)
   - Create a Dynamic Keyword Address in Windows Firewall
   - Create an outbound block rule referencing the keyword
   - Store record in audit log
   - Schedule periodic refresh task

3. **Scheduled Refresh**:
   - Background thread checks for due refresh tasks
   - Re-resolves FQDN to detect IP changes
   - Updates Dynamic Keyword Address if IPs changed
   - Updates audit log with new IPs

4. **Boot Pre-hydration**:
   - On startup, loads all existing records
   - Refreshes DNS for each blocked FQDN
   - Updates firewall rules with current IPs
   - Reschedules all refresh tasks

## Windows Firewall Platform Integration

This application uses the Windows Filtering Platform (WFP) API for firewall management:

- **Dynamic Keyword Addresses**: Named sets of IP addresses that can be updated dynamically
- **Firewall Filters**: Rules that reference keyword addresses to block/allow traffic
- **Auto-Resolution**: The scheduler periodically updates IP addresses to handle DNS changes

### Important Notes on WFP Implementation

The current implementation provides a **framework and simulation** of WFP Dynamic Keyword Address functionality. For a production deployment, you need to implement the full WFP API calls:

**Functions to Implement**:
- `FwpmDynamicKeywordSubscribe0()` - Register dynamic keywords
- `FwpmDynamicKeywordAddressAdd0()` - Add IP addresses to keywords
- `FwpmDynamicKeywordAddressUpdate0()` - Update IP addresses
- `FwpmDynamicKeywordAddressDelete0()` - Delete keyword addresses
- `FwpmFilterAdd0()` - Create firewall filters
- `FwpmFilterDeleteByKey0()` - Delete firewall filters

**References**:
- [Windows Filtering Platform Documentation](https://docs.microsoft.com/en-us/windows/win32/fwp/windows-filtering-platform-start-page)
- [Dynamic Keyword Addresses](https://docs.microsoft.com/en-us/windows/win32/fwp/fwpm-dynamic-keyword-address0)

## Logging and Auditing

### Log File

All operations are logged to `logs/fqdn_blocker.log` with timestamps:

```
2025-10-21 14:30:15 - Added record for FQDN: example.com with 2 IP(s)
2025-10-21 14:35:20 - Updated record for FQDN: example.com with 3 IP(s)
```

### Audit Store

The audit store (`data/audit_store.json`) maintains a JSON database of all blocked FQDNs:

```json
[
  {
    "fqdn": "example.com",
    "keywordId": "a1b2c3d4-e5f6-7890-abcd-ef1234567890",
    "ruleName": "Block example.com",
    "blockedAt": 1729520415,
    "interval": 60,
    "lastResolvedIPs": ["93.184.216.34", "2606:2800:220:1:248:1893:25c8:1946"]
  }
]
```

## Troubleshooting

### "This application requires Administrator privileges"

**Solution**: Right-click the executable and select "Run as Administrator"

### DNS Resolution Fails

**Possible Causes**:
- Network connectivity issues
- DNS server unavailable
- Invalid FQDN

**Solution**: Check network connection and verify FQDN is valid

### Firewall Manager Initialization Failed

**Possible Causes**:
- Insufficient privileges
- Windows Firewall service not running

**Solution**:
- Ensure running as Administrator
- Verify Windows Firewall service is enabled: `Get-Service mpssvc`

### Build Errors

**Missing json.hpp**:
- Download from: https://github.com/nlohmann/json/releases/latest/download/json.hpp
- Place in `lib/` directory

**CMake Errors**:
- Ensure CMake 3.15+ is installed
- Verify Visual Studio C++ tools are installed

## Future Enhancements

- [ ] Implement full WFP Dynamic Keyword Address API integration
- [ ] Add GUI front-end (Windows Forms or WPF)
- [ ] Support for inbound traffic blocking
- [ ] Import/export block lists
- [ ] Statistics and reporting dashboard
- [ ] Support for wildcard domain blocking (*.example.com)
- [ ] Integration with third-party block lists
- [ ] Service mode (run as Windows service)

## License

This project is provided as-is for educational and development purposes.

## Contributing

Contributions are welcome! Please ensure:
- Code follows C++17 standards
- All functions are documented
- Changes are tested on Windows 10/11
- Administrator privileges are properly handled

## Acknowledgments

- **nlohmann/json**: JSON for Modern C++ - https://github.com/nlohmann/json
- **Microsoft**: Windows Filtering Platform documentation and APIs

---

**Author**: Built with GitHub Copilot  
**Version**: 1.0.0  
**Last Updated**: October 2025
