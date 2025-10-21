#ifndef AUDITLOGGER_H
#define AUDITLOGGER_H

#include <string>
#include <vector>
#include <mutex>
#include <ctime>

/**
 * @brief Record structure for tracking blocked FQDNs
 */
struct Record {
    std::string fqdn;                      // Fully Qualified Domain Name
    std::string keywordId;                 // GUID for dynamic keyword address
    std::string ruleName;                  // Firewall rule name
    std::time_t blockedAt;                 // Timestamp when blocked
    std::vector<std::string> lastResolvedIPs;  // Last resolved IP addresses
    int interval;                          // Refresh interval in minutes

    /**
     * @brief Default constructor
     */
    Record();

    /**
     * @brief Parameterized constructor
     */
    Record(const std::string& fqdn, const std::string& keywordId, 
           const std::string& ruleName, const std::vector<std::string>& ips, int interval);
};

/**
 * @brief Audit logging and persistence management
 * 
 * Manages the audit store (JSON file) containing records of all blocked FQDNs.
 * Thread-safe for concurrent access.
 */
class AuditLogger {
public:
    /**
     * @brief Initialize the audit logger
     * @param auditStorePath Path to the audit store file
     */
    static void Initialize(const std::string& auditStorePath);

    /**
     * @brief Add a new record to the audit store
     * @param record Record to add
     * @return true if successful, false otherwise
     */
    static bool AddRecord(const Record& record);

    /**
     * @brief Update an existing record (primarily for IP updates)
     * @param fqdn FQDN to update
     * @param newIPs New IP addresses
     * @return true if successful, false otherwise
     */
    static bool UpdateRecord(const std::string& fqdn, const std::vector<std::string>& newIPs);

    /**
     * @brief List all records in the audit store
     * @return Vector of all records
     */
    static std::vector<Record> ListRecords();

    /**
     * @brief Get a specific record by FQDN
     * @param fqdn FQDN to search for
     * @param record Output parameter for the record
     * @return true if found, false otherwise
     */
    static bool GetRecord(const std::string& fqdn, Record& record);

    /**
     * @brief Remove a record from the audit store
     * @param fqdn FQDN to remove
     * @return true if successful, false otherwise
     */
    static bool RemoveRecord(const std::string& fqdn);

    /**
     * @brief Log an action to the log file
     * @param message Message to log
     */
    static void LogAction(const std::string& message);

private:
    /**
     * @brief Load all records from the audit store file
     * @return Vector of records
     */
    static std::vector<Record> LoadFromFile();

    /**
     * @brief Save all records to the audit store file
     * @param records Vector of records to save
     * @return true if successful, false otherwise
     */
    static bool SaveToFile(const std::vector<Record>& records);

    static std::string auditStorePath;
    static std::string logFilePath;
    static std::mutex auditMutex;  // For thread-safe access
};

#endif // AUDITLOGGER_H
