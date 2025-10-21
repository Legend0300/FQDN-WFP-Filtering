# Usage Examples

This document provides practical usage examples for the FQDN Blocker CLI.

## Prerequisites

Before running these examples:
1. Build the project (see BUILD.md)
2. Run as Administrator
3. Ensure Windows Firewall is enabled

## Basic Usage

### Example 1: Block a Single Domain

```powershell
# Block Facebook
.\FqdnBlockerCli.exe block facebook.com

# Output:
# Blocking FQDN: facebook.com
# Refresh interval: 60 minutes
# 
# Resolving facebook.com...
# Resolved facebook.com to 2 address(es)
# Creating dynamic keyword address...
# Creating firewall rule: Block facebook.com
# 
# Successfully blocked facebook.com
# Resolved to 2 IP address(es):
#   - 157.240.1.35
#   - 2a03:2880:f10c:83:face:b00c:0:25de
```

### Example 2: Block with Custom Interval

```powershell
# Block Twitter with 2-hour refresh interval
.\FqdnBlockerCli.exe block twitter.com 120

# Output:
# Blocking FQDN: twitter.com
# Refresh interval: 120 minutes
# ...
```

### Example 3: List All Blocks

```powershell
.\FqdnBlockerCli.exe list

# Output:
# ==================================================
# Blocked FQDNs (2)
# ==================================================
# 
# FQDN: facebook.com
#   Rule Name: Block facebook.com
#   Keyword ID: a1b2c3d4-e5f6-7890-abcd-ef1234567890
#   Refresh Interval: 60 minutes
#   IP Addresses (2):
#     - 157.240.1.35
#     - 2a03:2880:f10c:83:face:b00c:0:25de
#   Blocked At: 2025-10-21 14:30:15
# 
# FQDN: twitter.com
#   Rule Name: Block twitter.com
#   Keyword ID: b2c3d4e5-f6a7-8901-bcde-f12345678901
#   Refresh Interval: 120 minutes
#   IP Addresses (3):
#     - 104.244.42.1
#     - 104.244.42.65
#     - 2606:1f80:1:1::1
#   Blocked At: 2025-10-21 14:35:20
# ==================================================
```

### Example 4: Manually Refresh All Blocks

```powershell
.\FqdnBlockerCli.exe refresh

# Output:
# Refreshing all blocked FQDNs...
# 
# Refreshing: facebook.com
# Resolved facebook.com to 2 address(es)
#   No changes detected
# 
# Refreshing: twitter.com
# Resolved twitter.com to 3 address(es)
#   IP addresses changed
#   Updated successfully
# 
# ==================================================
# Refresh complete: 2 successful, 0 failed
```

### Example 5: Remove a Block

```powershell
.\FqdnBlockerCli.exe remove facebook.com

# Output:
# Removing block for: facebook.com
# Deleting firewall rule...
# Deleting dynamic keyword address...
# 
# Successfully removed block for: facebook.com
```

## Advanced Scenarios

### Scenario 1: Block Social Media Platforms

```powershell
# Block multiple social media sites
.\FqdnBlockerCli.exe block facebook.com 60
.\FqdnBlockerCli.exe block twitter.com 60
.\FqdnBlockerCli.exe block instagram.com 60
.\FqdnBlockerCli.exe block tiktok.com 60
.\FqdnBlockerCli.exe block reddit.com 60

# Verify
.\FqdnBlockerCli.exe list
```

### Scenario 2: Block Advertising Servers

```powershell
# Block common ad servers with shorter refresh (30 min)
.\FqdnBlockerCli.exe block doubleclick.net 30
.\FqdnBlockerCli.exe block googleadservices.com 30
.\FqdnBlockerCli.exe block googlesyndication.com 30
```

### Scenario 3: Parental Controls

```powershell
# Block adult content and gaming sites
.\FqdnBlockerCli.exe block youtube.com 120
.\FqdnBlockerCli.exe block twitch.tv 120
.\FqdnBlockerCli.exe block discord.com 120

# Set a longer default interval for parental controls
.\FqdnBlockerCli.exe set-interval 180
```

### Scenario 4: Productivity Blocking

```powershell
# Block distracting sites during work hours
.\FqdnBlockerCli.exe block youtube.com 60
.\FqdnBlockerCli.exe block reddit.com 60
.\FqdnBlockerCli.exe block netflix.com 60

# Later, remove blocks after work
.\FqdnBlockerCli.exe remove youtube.com
.\FqdnBlockerCli.exe remove reddit.com
.\FqdnBlockerCli.exe remove netflix.com
```

### Scenario 5: Malware/Phishing Protection

```powershell
# Block known malicious domains (examples only)
.\FqdnBlockerCli.exe block malicious-site.com 30
.\FqdnBlockerCli.exe block phishing-site.com 30
.\FqdnBlockerCli.exe block scam-site.com 30

# Use shorter intervals for threat domains
```

## Configuration Examples

### Example 1: Change Default Refresh Interval

```powershell
# Set default to 2 hours (120 minutes)
.\FqdnBlockerCli.exe set-interval 120

# New blocks will use this interval by default
.\FqdnBlockerCli.exe block example.com
# (uses 120-minute interval)
```

### Example 2: Custom Configuration File

Edit `config/config.json`:

```json
{
  "defaultInterval": 30,
  "logFilePath": "C:\\Logs\\fqdn_blocker.log",
  "auditStorePath": "C:\\Data\\blocks.json"
}
```

Then restart the application to load new settings.

## Scheduled Operation

### Example 1: Let Scheduler Run

```powershell
# Block a domain with auto-refresh
.\FqdnBlockerCli.exe block example.com 60

# The application will:
# 1. Block the domain immediately
# 2. Start the scheduler
# 3. Refresh DNS every 60 minutes automatically
# 4. Continue until Ctrl+C is pressed

# Output:
# Successfully blocked example.com
# 
# Starting scheduler for 1 task(s)...
# Press Ctrl+C to stop.
# 
# [Scheduler] Triggering refresh for: example.com
# Resolved example.com to 2 address(es)
# [Scheduler] No IP changes detected for: example.com
# [Scheduler] Next refresh in 60 minutes
```

### Example 2: Background Operation

For continuous background operation, consider:

1. **Option A**: Run in a separate PowerShell window
   - Start a new PowerShell window as Administrator
   - Run the blocker
   - Minimize the window

2. **Option B**: Create a scheduled task (Windows Task Scheduler)
   - Create a task to run at startup
   - Run with highest privileges
   - Configure to run even when not logged in

## Troubleshooting Examples

### Example 1: Check if Domain is Blocked

```powershell
# List all blocks and search for specific domain
.\FqdnBlockerCli.exe list | Select-String "example.com"
```

### Example 2: Verify DNS Resolution

Before blocking, test DNS resolution:

```powershell
# Windows nslookup
nslookup example.com

# PowerShell
Resolve-DnsName example.com
```

### Example 3: Force Refresh After DNS Changes

If you know DNS has changed:

```powershell
# Manually trigger refresh
.\FqdnBlockerCli.exe refresh
```

### Example 4: Clear All Blocks

PowerShell script to remove all blocks:

```powershell
# Get all blocked domains and remove them
.\FqdnBlockerCli.exe list | Select-String "FQDN:" | ForEach-Object {
    if ($_ -match "FQDN:\s+(.+)") {
        $fqdn = $matches[1]
        Write-Host "Removing: $fqdn"
        .\FqdnBlockerCli.exe remove $fqdn
    }
}
```

## Testing Examples

### Example 1: Test with Safe Domains

```powershell
# Block a test domain
.\FqdnBlockerCli.exe block example.com 5

# Wait 5 minutes and observe refresh
# (example.com is safe for testing)

# Remove when done
.\FqdnBlockerCli.exe remove example.com
```

### Example 2: Verify Blocking Works

After blocking a domain:

```powershell
# Try to ping (should fail or timeout)
ping facebook.com

# Try to access in browser
# (should not connect if properly blocked)
```

## Batch Operations

### Example 1: Block Multiple Domains from File

Create `domains.txt`:
```
facebook.com
twitter.com
instagram.com
reddit.com
```

PowerShell script:
```powershell
Get-Content domains.txt | ForEach-Object {
    Write-Host "Blocking: $_"
    .\FqdnBlockerCli.exe block $_ 60
}
```

### Example 2: Export Block List

```powershell
# Export to text file
.\FqdnBlockerCli.exe list > blocked_domains.txt
```

### Example 3: Backup Audit Store

```powershell
# Copy audit store for backup
Copy-Item "data\audit_store.json" "backup\audit_store_$(Get-Date -Format 'yyyyMMdd').json"
```

## Integration Examples

### Example 1: Scheduled Task Script

Create `daily_refresh.ps1`:
```powershell
# Navigate to application directory
cd "C:\Path\To\FqdnBlockerCli\build"

# Run refresh
.\FqdnBlockerCli.exe refresh

# Log the action
Add-Content "C:\Logs\daily_refresh.log" "$(Get-Date): Refresh completed"
```

Schedule this script to run daily.

### Example 2: Email Notification on Changes

PowerShell script to check logs and email if changes detected:
```powershell
$logFile = "logs\fqdn_blocker.log"
$lastCheck = (Get-Date).AddHours(-1)

$changes = Get-Content $logFile | Where-Object {
    $_ -match "Updated record" -and 
    [DateTime]::ParseExact($_.Substring(0,19), "yyyy-MM-dd HH:mm:ss", $null) -gt $lastCheck
}

if ($changes) {
    Send-MailMessage -To "admin@example.com" `
                     -From "blocker@example.com" `
                     -Subject "DNS Changes Detected" `
                     -Body ($changes -join "`n") `
                     -SmtpServer "smtp.example.com"
}
```

## Best Practices

### 1. Start with Long Intervals

```powershell
# Use longer intervals initially (e.g., 2-4 hours)
.\FqdnBlockerCli.exe block example.com 180
```

### 2. Regular Maintenance

```powershell
# Weekly: Review blocked domains
.\FqdnBlockerCli.exe list

# Monthly: Manually refresh all
.\FqdnBlockerCli.exe refresh
```

### 3. Keep Logs

```powershell
# Periodically archive logs
$date = Get-Date -Format "yyyyMMdd"
Copy-Item "logs\fqdn_blocker.log" "archive\fqdn_blocker_$date.log"
```

### 4. Test Before Production

```powershell
# Always test with non-critical domains first
.\FqdnBlockerCli.exe block test.example.com 5
# Verify it works
.\FqdnBlockerCli.exe list
# Remove test
.\FqdnBlockerCli.exe remove test.example.com
```

## Common Use Cases

### Parental Controls
- Block age-inappropriate content
- Block gaming/streaming during homework hours
- Monitor via logs

### Corporate Environment
- Block social media during work hours
- Block file sharing sites
- Block malware/phishing domains

### Privacy Protection
- Block tracking/analytics domains
- Block advertising networks
- Block data collection services

### Network Security
- Block known malicious domains
- Block C&C servers
- Block phishing sites

---

For more information, see:
- Main documentation: [README.md](README.md)
- API reference: [API.md](API.md)
- Build instructions: [BUILD.md](BUILD.md)
