# Logs Directory

This directory contains log files for the FQDN Blocker CLI application.

## Log File

**File**: `fqdn_blocker.log`

This file is automatically created when the application performs its first logged action.

## Log Format

Logs are written with timestamps in the following format:

```
YYYY-MM-DD HH:MM:SS - Message
```

## Example Log Entries

```
2025-10-21 14:30:15 - Added record for FQDN: example.com with 2 IP(s)
2025-10-21 14:35:20 - Updated record for FQDN: example.com with 3 IP(s)
2025-10-21 15:00:00 - Removed record for FQDN: example.com
```

## Logged Actions

The application logs the following events:

- **Block operations**: When a new FQDN is blocked
- **Update operations**: When IP addresses are refreshed
- **Remove operations**: When a block is removed
- **Configuration changes**: When settings are modified

## Maintenance

- Log files are appended to (not overwritten)
- Consider rotating or archiving logs periodically
- No automatic log rotation is implemented
- Safe to delete when not running the application

## Location

By default: `logs/fqdn_blocker.log`

Can be configured in `config/config.json`:
```json
{
  "logFilePath": "logs/fqdn_blocker.log"
}
```
