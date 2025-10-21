# Audit Store Directory

This directory will contain the audit store file (`audit_store.json`) that maintains the database of all blocked FQDNs.

The file is automatically created when you block your first FQDN.

## File Structure

The audit store is a JSON file with the following structure:

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

## Fields

- **fqdn**: The Fully Qualified Domain Name being blocked
- **keywordId**: GUID of the Windows Firewall dynamic keyword address
- **ruleName**: Name of the firewall rule
- **blockedAt**: Unix timestamp when the block was created
- **interval**: Refresh interval in minutes
- **lastResolvedIPs**: Array of IP addresses (IPv4 and IPv6) last resolved for this FQDN

## Important Notes

- Do not manually edit this file while the application is running
- Backup this file to preserve your block list
- The file is read on application startup for boot pre-hydration
- Each record corresponds to one blocked FQDN
