# FQDN Blocker CLI - API Documentation

This document provides detailed information about the internal APIs and modules of the FQDN Blocker CLI application.

## Module Overview

The application is structured into six main modules:

1. **Config** - Configuration management
2. **AuditLogger** - Audit logging and persistence
3. **Resolver** - DNS resolution
4. **FirewallManager** - Windows Firewall Platform integration
5. **Scheduler** - Background task scheduling
6. **Main** - CLI interface and command handling

---

## 1. Config Module

**Files**: `Config.h`, `Config.cpp`

### Purpose
Manages application configuration including default intervals, file paths, and settings persistence.

### Static Methods

#### `bool Load(const std::string& configPath)`
Loads configuration from a JSON file.

**Parameters**:
- `configPath`: Path to the configuration file

**Returns**: `true` if successful, `false` otherwise

**Example**:
```cpp
Config::Load("config/config.json");
```

#### `bool Save(const std::string& configPath)`
Saves current configuration to a JSON file.

**Parameters**:
- `configPath`: Path to save the configuration

**Returns**: `true` if successful, `false` otherwise

#### `int GetDefaultInterval()`
Gets the default refresh interval in minutes.

**Returns**: Interval in minutes

#### `void SetDefaultInterval(int intervalMinutes)`
Sets the default refresh interval.

**Parameters**:
- `intervalMinutes`: New interval in minutes

#### `std::string GetLogFilePath()`
Gets the log file path.

**Returns**: Path to log file

#### `std::string GetAuditStorePath()`
Gets the audit store path.

**Returns**: Path to audit store file

---

## 2. AuditLogger Module

**Files**: `AuditLogger.h`, `AuditLogger.cpp`

### Purpose
Manages the audit store (JSON database) and logging of all operations.

### Record Structure

```cpp
struct Record {
    std::string fqdn;                      // Domain name
    std::string keywordId;                 // GUID
    std::string ruleName;                  // Firewall rule name
    std::time_t blockedAt;                 // Timestamp
    std::vector<std::string> lastResolvedIPs;  // IP addresses
    int interval;                          // Refresh interval (minutes)
};
```

### Static Methods

#### `void Initialize(const std::string& auditStorePath)`
Initializes the audit logger with the audit store path.

**Parameters**:
- `auditStorePath`: Path to the audit store JSON file

**Thread Safety**: Thread-safe

#### `bool AddRecord(const Record& record)`
Adds a new record to the audit store.

**Parameters**:
- `record`: Record to add

**Returns**: `true` if successful, `false` if record already exists

**Thread Safety**: Thread-safe

**Example**:
```cpp
Record record("example.com", guidStr, "Block example.com", ips, 60);
AuditLogger::AddRecord(record);
```

#### `bool UpdateRecord(const std::string& fqdn, const std::vector<std::string>& newIPs)`
Updates the IP addresses for an existing record.

**Parameters**:
- `fqdn`: FQDN to update
- `newIPs`: New IP addresses

**Returns**: `true` if successful, `false` if not found

**Thread Safety**: Thread-safe

#### `std::vector<Record> ListRecords()`
Retrieves all records from the audit store.

**Returns**: Vector of all records

**Thread Safety**: Thread-safe

#### `bool GetRecord(const std::string& fqdn, Record& record)`
Retrieves a specific record by FQDN.

**Parameters**:
- `fqdn`: FQDN to search for
- `record`: Output parameter for the record

**Returns**: `true` if found, `false` otherwise

**Thread Safety**: Thread-safe

#### `bool RemoveRecord(const std::string& fqdn)`
Removes a record from the audit store.

**Parameters**:
- `fqdn`: FQDN to remove

**Returns**: `true` if successful, `false` if not found

**Thread Safety**: Thread-safe

#### `void LogAction(const std::string& message)`
Logs a message to the log file with timestamp.

**Parameters**:
- `message`: Message to log

**Thread Safety**: Thread-safe

---

## 3. Resolver Module

**Files**: `Resolver.h`, `Resolver.cpp`

### Purpose
Provides DNS resolution functionality using Windows Winsock APIs.

### Static Methods

#### `std::vector<std::string> ResolveFqdn(const std::string& fqdn)`
Resolves an FQDN to a list of IP addresses.

**Parameters**:
- `fqdn`: Domain name to resolve

**Returns**: Vector of IP address strings (IPv4 and IPv6)

**Example**:
```cpp
auto ips = Resolver::ResolveFqdn("example.com");
for (const auto& ip : ips) {
    std::cout << ip << std::endl;
}
```

**Notes**:
- Uses `getaddrinfo()` internally
- Returns both IPv4 and IPv6 addresses
- Returns empty vector on failure

#### `bool IsAvailable()`
Checks if DNS resolution is available.

**Returns**: `true` if Winsock is available, `false` otherwise

---

## 4. FirewallManager Module

**Files**: `FirewallManager.h`, `FirewallManager.cpp`

### Purpose
Wraps Windows Filtering Platform (WFP) APIs for firewall rule management.

### Static Methods

#### `bool Initialize()`
Initializes the Firewall Manager and opens a session to the WFP engine.

**Returns**: `true` if successful, `false` otherwise

**Notes**:
- Must be called before any other firewall operations
- Requires Administrator privileges

#### `void Cleanup()`
Closes the WFP session and cleans up resources.

#### `std::string CreateDynamicKeywordAddress(const std::string& fqdn, const std::vector<std::string>& ips, bool autoResolve = true)`
Creates a dynamic keyword address object in the firewall.

**Parameters**:
- `fqdn`: FQDN to use as keyword name
- `ips`: Initial IP addresses
- `autoResolve`: Enable auto-resolution (for future use)

**Returns**: GUID of the created keyword address, or empty string on failure

**Example**:
```cpp
std::string guid = FirewallManager::CreateDynamicKeywordAddress("example.com", ips, true);
```

#### `bool UpdateDynamicKeywordAddress(const std::string& keywordId, const std::vector<std::string>& ips)`
Updates the IP addresses associated with a dynamic keyword.

**Parameters**:
- `keywordId`: GUID of the keyword address
- `ips`: New IP addresses

**Returns**: `true` if successful, `false` otherwise

#### `bool DeleteDynamicKeywordAddress(const std::string& keywordId)`
Deletes a dynamic keyword address.

**Parameters**:
- `keywordId`: GUID of the keyword to delete

**Returns**: `true` if successful, `false` otherwise

#### `bool CreateFirewallRule(const std::string& ruleName, const std::string& keywordId, const std::string& direction, const std::string& action)`
Creates a firewall rule that references a dynamic keyword address.

**Parameters**:
- `ruleName`: Name of the rule
- `keywordId`: GUID of the dynamic keyword address
- `direction`: "Outbound" or "Inbound"
- `action`: "Block" or "Allow"

**Returns**: `true` if successful, `false` otherwise

**Example**:
```cpp
FirewallManager::CreateFirewallRule("Block example.com", guid, "Outbound", "Block");
```

#### `bool DeleteFirewallRule(const std::string& ruleName)`
Deletes a firewall rule by name.

**Parameters**:
- `ruleName`: Name of the rule to delete

**Returns**: `true` if successful, `false` otherwise

#### `std::string GenerateGUID()`
Generates a new GUID string.

**Returns**: GUID in string format (lowercase hex)

**Example**: `"a1b2c3d4-e5f6-7890-abcd-ef1234567890"`

---

## 5. Scheduler Module

**Files**: `Scheduler.h`, `Scheduler.cpp`

### Purpose
Manages background task scheduling for periodic DNS refresh operations.

### Static Methods

#### `void Initialize()`
Initializes the scheduler.

#### `void Start()`
Starts the scheduler event loop in a background thread.

**Notes**:
- Non-blocking - runs in separate thread
- Only starts if not already running

#### `void Stop()`
Stops the scheduler and waits for the background thread to finish.

#### `bool AddTask(const std::string& fqdn, int intervalMinutes)`
Schedules a periodic refresh task for an FQDN.

**Parameters**:
- `fqdn`: Domain name to refresh
- `intervalMinutes`: Refresh interval in minutes

**Returns**: `true` if successful

**Thread Safety**: Thread-safe

**Example**:
```cpp
Scheduler::AddTask("example.com", 60);  // Refresh every 60 minutes
```

#### `bool RemoveTask(const std::string& fqdn)`
Removes a scheduled task.

**Parameters**:
- `fqdn`: Domain name to stop refreshing

**Returns**: `true` if task was found and removed, `false` otherwise

**Thread Safety**: Thread-safe

#### `int GetTaskCount()`
Gets the number of active scheduled tasks.

**Returns**: Number of tasks

**Thread Safety**: Thread-safe

#### `bool IsRunning()`
Checks if the scheduler is currently running.

**Returns**: `true` if running, `false` otherwise

### Internal Behavior

The scheduler:
- Checks for due tasks every 10 seconds
- Triggers DNS resolution for due FQDNs
- Compares new IPs with stored IPs
- Updates firewall rules if IPs changed
- Reschedules tasks for next interval

---

## 6. Main Module

**File**: `main.cpp`

### Purpose
Provides the CLI interface and command handling logic.

### Command Handlers

#### `void HandleBlockCommand(int argc, char* argv[])`
Handles the "block" command.

**Flow**:
1. Parse FQDN and optional interval
2. Resolve FQDN to IPs
3. Create dynamic keyword address
4. Create firewall rule
5. Add to audit logger
6. Schedule refresh task

#### `void HandleRefreshCommand()`
Handles the "refresh" command.

**Flow**:
1. Load all records
2. For each record:
   - Resolve FQDN
   - Compare IPs
   - Update if changed

#### `void HandleListCommand()`
Handles the "list" command.

**Flow**:
1. Load all records
2. Display formatted output for each

#### `void HandleRemoveCommand(int argc, char* argv[])`
Handles the "remove" command.

**Flow**:
1. Parse FQDN
2. Get record from audit logger
3. Delete firewall rule
4. Delete dynamic keyword address
5. Remove from audit logger
6. Remove from scheduler

#### `void HandleSetIntervalCommand(int argc, char* argv[])`
Handles the "set-interval" command.

**Flow**:
1. Parse interval value
2. Update configuration
3. Save configuration

### Utility Functions

#### `void PrintUsage()`
Displays usage information and help text.

#### `void PerformBootPreHydration()`
Performs DNS refresh for all existing records on application startup.

**Flow**:
1. Load all records
2. For each record:
   - Resolve FQDN
   - Update dynamic keyword address
   - Update audit record
   - Re-add to scheduler

#### `bool IsAdministrator()`
Checks if the application is running with Administrator privileges.

**Returns**: `true` if running as admin, `false` otherwise

---

## Thread Safety

### Thread-Safe Modules
- **AuditLogger**: Uses `std::mutex` for all file operations
- **Scheduler**: Uses `std::mutex` for task management

### Not Thread-Safe
- **Config**: Designed for single-threaded initialization
- **Resolver**: Stateless, safe to call from multiple threads
- **FirewallManager**: Should be called from main thread only

---

## Error Handling

### Error Reporting
- Methods return `bool` for success/failure
- Error messages printed to `std::cerr`
- Actions logged to log file

### Exception Safety
- All modules use try-catch blocks
- Exceptions caught and converted to error returns
- No exceptions propagate to main()

---

## Dependencies

### Standard Library
- `<string>`, `<vector>`, `<map>`
- `<mutex>`, `<thread>`, `<atomic>`
- `<chrono>` (for timing)
- `<fstream>`, `<iostream>` (for I/O)

### Windows APIs
- Winsock2 (`ws2_32.lib`)
- IP Helper API (`iphlpapi.lib`)
- Windows Filtering Platform (`fwpuclnt.lib`)
- RPC Runtime (`rpcrt4.lib`)
- Advanced Windows API (`advapi32.lib`)

### Third-Party
- nlohmann/json (header-only)

---

## Future API Extensions

### Planned Additions
1. **Callback Support**: Register callbacks for DNS updates
2. **Batch Operations**: Block/remove multiple FQDNs at once
3. **Import/Export**: Backup and restore block lists
4. **Statistics API**: Query block counts, hit counts, etc.
5. **Wildcard Support**: Block `*.example.com`

### Extensibility
The modular design allows for:
- GUI front-end integration
- Service mode operation
- Remote management API
- Plugin system for custom actions

---

## Performance Considerations

### Optimization Tips
1. **Batch operations**: Group multiple DNS resolutions
2. **Caching**: Consider caching DNS results
3. **Async operations**: Use async I/O for file operations
4. **Thread pooling**: For parallel DNS resolution

### Scalability
- Current design handles hundreds of FQDNs
- For thousands, consider:
  - Database instead of JSON
  - More efficient data structures
  - Distributed architecture

---

## Security Considerations

### Administrator Privileges
- Required for firewall operations
- Check at startup: `IsAdministrator()`
- UAC prompt on Windows

### Input Validation
- FQDN validation (RFC compliance)
- IP address validation
- Parameter bounds checking

### Data Protection
- Audit store in protected directory
- Log files with restricted permissions
- No sensitive data in memory dumps

---

For implementation examples and usage, see the main [README.md](README.md) and [BUILD.md](BUILD.md).
