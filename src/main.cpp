#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <Windows.h>

#include "Config.h"
#include "AuditLogger.h"
#include "FirewallManager.h"
#include "Resolver.h"
#include "Scheduler.h"

// Function declarations
void PrintUsage();
void HandleBlockCommand(int argc, char* argv[]);
void HandleRefreshCommand();
void HandleListCommand();
void HandleRemoveCommand(int argc, char* argv[]);
void HandleSetIntervalCommand(int argc, char* argv[]);
void PerformBootPreHydration();
bool IsAdministrator();

int main(int argc, char* argv[]) {
    std::cout << "==================================================" << std::endl;
    std::cout << "     FQDN Blocker CLI - Windows Firewall Tool    " << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;

    // Check for Administrator privileges
    if (!IsAdministrator()) {
        std::cerr << "ERROR: This application requires Administrator privileges." << std::endl;
        std::cerr << "Please run as Administrator." << std::endl;
        return 1;
    }

    // Load configuration
    std::string configPath = "config/config.json";
    Config::Load(configPath);

    // Initialize components
    AuditLogger::Initialize(Config::GetAuditStorePath());
    
    if (!FirewallManager::Initialize()) {
        std::cerr << "Failed to initialize Firewall Manager" << std::endl;
        return 1;
    }

    Scheduler::Initialize();

    // Perform boot pre-hydration
    PerformBootPreHydration();

    // Parse command line arguments
    if (argc < 2) {
        PrintUsage();
        FirewallManager::Cleanup();
        return 1;
    }

    std::string command = argv[1];

    try {
        if (command == "block") {
            HandleBlockCommand(argc, argv);
        }
        else if (command == "refresh") {
            HandleRefreshCommand();
        }
        else if (command == "list") {
            HandleListCommand();
        }
        else if (command == "remove") {
            HandleRemoveCommand(argc, argv);
        }
        else if (command == "set-interval") {
            HandleSetIntervalCommand(argc, argv);
        }
        else if (command == "help" || command == "--help" || command == "-h") {
            PrintUsage();
        }
        else {
            std::cerr << "Unknown command: " << command << std::endl;
            PrintUsage();
            FirewallManager::Cleanup();
            return 1;
        }

        // If there are scheduled tasks, start the scheduler
        if (Scheduler::GetTaskCount() > 0 && !Scheduler::IsRunning()) {
            std::cout << "\nStarting scheduler for " << Scheduler::GetTaskCount() << " task(s)..." << std::endl;
            std::cout << "Press Ctrl+C to stop." << std::endl;
            Scheduler::Start();

            // Keep the application running
            while (Scheduler::IsRunning()) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        FirewallManager::Cleanup();
        return 1;
    }

    // Cleanup
    Scheduler::Stop();
    FirewallManager::Cleanup();

    return 0;
}

void PrintUsage() {
    std::cout << "Usage: FqdnBlockerCli <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  block <fqdn> [interval]    Block an FQDN with optional refresh interval (minutes)" << std::endl;
    std::cout << "                             Example: FqdnBlockerCli block example.com 60" << std::endl;
    std::cout << std::endl;
    std::cout << "  refresh                    Manually refresh all blocked FQDNs" << std::endl;
    std::cout << "                             Example: FqdnBlockerCli refresh" << std::endl;
    std::cout << std::endl;
    std::cout << "  list                       List all blocked FQDNs and their status" << std::endl;
    std::cout << "                             Example: FqdnBlockerCli list" << std::endl;
    std::cout << std::endl;
    std::cout << "  remove <fqdn>              Remove a blocked FQDN and its firewall rule" << std::endl;
    std::cout << "                             Example: FqdnBlockerCli remove example.com" << std::endl;
    std::cout << std::endl;
    std::cout << "  set-interval <minutes>     Set the default refresh interval" << std::endl;
    std::cout << "                             Example: FqdnBlockerCli set-interval 120" << std::endl;
    std::cout << std::endl;
    std::cout << "  help                       Display this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Note: This application requires Administrator privileges." << std::endl;
}

void HandleBlockCommand(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Error: Missing FQDN parameter" << std::endl;
        std::cout << "Usage: FqdnBlockerCli block <fqdn> [interval]" << std::endl;
        return;
    }

    std::string fqdn = argv[2];
    int interval = Config::GetDefaultInterval();

    if (argc >= 4) {
        try {
            interval = std::stoi(argv[3]);
            if (interval <= 0) {
                std::cerr << "Error: Interval must be a positive number" << std::endl;
                return;
            }
        }
        catch (const std::exception&) {
            std::cerr << "Error: Invalid interval value" << std::endl;
            return;
        }
    }

    std::cout << "\nBlocking FQDN: " << fqdn << std::endl;
    std::cout << "Refresh interval: " << interval << " minutes" << std::endl;
    std::cout << std::endl;

    // Resolve the FQDN
    std::cout << "Resolving " << fqdn << "..." << std::endl;
    std::vector<std::string> ips = Resolver::ResolveFqdn(fqdn);

    if (ips.empty()) {
        std::cerr << "Error: Could not resolve FQDN" << std::endl;
        return;
    }

    // Create dynamic keyword address
    std::cout << "Creating dynamic keyword address..." << std::endl;
    std::string keywordId = FirewallManager::CreateDynamicKeywordAddress(fqdn, ips, true);

    if (keywordId.empty()) {
        std::cerr << "Error: Failed to create dynamic keyword address" << std::endl;
        return;
    }

    // Create firewall rule
    std::string ruleName = "Block " + fqdn;
    std::cout << "Creating firewall rule: " << ruleName << std::endl;
    
    if (!FirewallManager::CreateFirewallRule(ruleName, keywordId, "Outbound", "Block")) {
        std::cerr << "Error: Failed to create firewall rule" << std::endl;
        FirewallManager::DeleteDynamicKeywordAddress(keywordId);
        return;
    }

    // Add to audit logger
    Record record(fqdn, keywordId, ruleName, ips, interval);
    if (!AuditLogger::AddRecord(record)) {
        std::cerr << "Error: Failed to add audit record" << std::endl;
        FirewallManager::DeleteFirewallRule(ruleName);
        FirewallManager::DeleteDynamicKeywordAddress(keywordId);
        return;
    }

    // Add to scheduler
    Scheduler::AddTask(fqdn, interval);

    std::cout << "\nSuccessfully blocked " << fqdn << std::endl;
    std::cout << "Resolved to " << ips.size() << " IP address(es):" << std::endl;
    for (const auto& ip : ips) {
        std::cout << "  - " << ip << std::endl;
    }
}

void HandleRefreshCommand() {
    std::cout << "\nRefreshing all blocked FQDNs..." << std::endl;

    auto records = AuditLogger::ListRecords();

    if (records.empty()) {
        std::cout << "No FQDNs are currently blocked." << std::endl;
        return;
    }

    int successCount = 0;
    int failureCount = 0;

    for (const auto& record : records) {
        std::cout << "\nRefreshing: " << record.fqdn << std::endl;

        // Resolve new IPs
        std::vector<std::string> newIPs = Resolver::ResolveFqdn(record.fqdn);

        if (newIPs.empty()) {
            std::cerr << "  Failed to resolve" << std::endl;
            failureCount++;
            continue;
        }

        // Check if IPs changed
        bool changed = (newIPs.size() != record.lastResolvedIPs.size());
        if (!changed) {
            std::vector<std::string> sortedNew = newIPs;
            std::vector<std::string> sortedOld = record.lastResolvedIPs;
            std::sort(sortedNew.begin(), sortedNew.end());
            std::sort(sortedOld.begin(), sortedOld.end());
            changed = (sortedNew != sortedOld);
        }

        if (changed) {
            std::cout << "  IP addresses changed" << std::endl;
            
            // Update dynamic keyword address
            if (FirewallManager::UpdateDynamicKeywordAddress(record.keywordId, newIPs)) {
                // Update audit record
                AuditLogger::UpdateRecord(record.fqdn, newIPs);
                std::cout << "  Updated successfully" << std::endl;
                successCount++;
            }
            else {
                std::cerr << "  Failed to update firewall" << std::endl;
                failureCount++;
            }
        }
        else {
            std::cout << "  No changes detected" << std::endl;
            successCount++;
        }
    }

    std::cout << "\n==================================================" << std::endl;
    std::cout << "Refresh complete: " << successCount << " successful, " 
              << failureCount << " failed" << std::endl;
}

void HandleListCommand() {
    auto records = AuditLogger::ListRecords();

    if (records.empty()) {
        std::cout << "\nNo FQDNs are currently blocked." << std::endl;
        return;
    }

    std::cout << "\n==================================================" << std::endl;
    std::cout << "Blocked FQDNs (" << records.size() << ")" << std::endl;
    std::cout << "==================================================" << std::endl;

    for (const auto& record : records) {
        std::cout << "\nFQDN: " << record.fqdn << std::endl;
        std::cout << "  Rule Name: " << record.ruleName << std::endl;
        std::cout << "  Keyword ID: " << record.keywordId << std::endl;
        std::cout << "  Refresh Interval: " << record.interval << " minutes" << std::endl;
        std::cout << "  IP Addresses (" << record.lastResolvedIPs.size() << "):" << std::endl;
        
        for (const auto& ip : record.lastResolvedIPs) {
            std::cout << "    - " << ip << std::endl;
        }

        // Convert blocked time
        std::tm tm;
        localtime_s(&tm, &record.blockedAt);
        std::cout << "  Blocked At: " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << std::endl;
    }

    std::cout << "==================================================" << std::endl;
}

void HandleRemoveCommand(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Error: Missing FQDN parameter" << std::endl;
        std::cout << "Usage: FqdnBlockerCli remove <fqdn>" << std::endl;
        return;
    }

    std::string fqdn = argv[2];

    std::cout << "\nRemoving block for: " << fqdn << std::endl;

    // Get the record
    Record record;
    if (!AuditLogger::GetRecord(fqdn, record)) {
        std::cerr << "Error: FQDN not found in blocked list" << std::endl;
        return;
    }

    // Delete firewall rule
    std::cout << "Deleting firewall rule..." << std::endl;
    if (!FirewallManager::DeleteFirewallRule(record.ruleName)) {
        std::cerr << "Warning: Failed to delete firewall rule" << std::endl;
    }

    // Delete dynamic keyword address
    std::cout << "Deleting dynamic keyword address..." << std::endl;
    if (!FirewallManager::DeleteDynamicKeywordAddress(record.keywordId)) {
        std::cerr << "Warning: Failed to delete dynamic keyword address" << std::endl;
    }

    // Remove from audit logger
    if (!AuditLogger::RemoveRecord(fqdn)) {
        std::cerr << "Error: Failed to remove audit record" << std::endl;
        return;
    }

    // Remove from scheduler
    Scheduler::RemoveTask(fqdn);

    std::cout << "\nSuccessfully removed block for: " << fqdn << std::endl;
}

void HandleSetIntervalCommand(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Error: Missing interval parameter" << std::endl;
        std::cout << "Usage: FqdnBlockerCli set-interval <minutes>" << std::endl;
        return;
    }

    try {
        int interval = std::stoi(argv[2]);
        
        if (interval <= 0) {
            std::cerr << "Error: Interval must be a positive number" << std::endl;
            return;
        }

        Config::SetDefaultInterval(interval);
        Config::Save("config/config.json");

        std::cout << "\nDefault refresh interval set to: " << interval << " minutes" << std::endl;
    }
    catch (const std::exception&) {
        std::cerr << "Error: Invalid interval value" << std::endl;
    }
}

void PerformBootPreHydration() {
    auto records = AuditLogger::ListRecords();

    if (records.empty()) {
        return;
    }

    std::cout << "\n==================================================" << std::endl;
    std::cout << "Performing boot pre-hydration for " << records.size() << " FQDN(s)..." << std::endl;
    std::cout << "==================================================" << std::endl;

    for (const auto& record : records) {
        std::cout << "\nHydrating: " << record.fqdn << std::endl;

        // Resolve the FQDN
        std::vector<std::string> ips = Resolver::ResolveFqdn(record.fqdn);

        if (ips.empty()) {
            std::cerr << "  Warning: Failed to resolve" << std::endl;
            continue;
        }

        // Update the dynamic keyword address
        if (FirewallManager::UpdateDynamicKeywordAddress(record.keywordId, ips)) {
            AuditLogger::UpdateRecord(record.fqdn, ips);
            std::cout << "  Successfully hydrated with " << ips.size() << " IP(s)" << std::endl;
        }
        else {
            std::cerr << "  Warning: Failed to update firewall" << std::endl;
        }

        // Re-add to scheduler
        Scheduler::AddTask(record.fqdn, record.interval);
    }

    std::cout << "\nBoot pre-hydration complete." << std::endl;
    std::cout << "==================================================" << std::endl;
}

bool IsAdministrator() {
    BOOL isAdmin = FALSE;
    PSID administratorsGroup = nullptr;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&ntAuthority, 2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &administratorsGroup)) {
        
        if (!CheckTokenMembership(nullptr, administratorsGroup, &isAdmin)) {
            isAdmin = FALSE;
        }
        FreeSid(administratorsGroup);
    }

    return isAdmin == TRUE;
}
