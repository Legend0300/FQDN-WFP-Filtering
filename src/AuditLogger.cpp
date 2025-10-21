#include "AuditLogger.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>

#include "../lib/json.hpp"

using json = nlohmann::json;

// Initialize static members
std::string AuditLogger::auditStorePath;
std::string AuditLogger::logFilePath;
std::mutex AuditLogger::auditMutex;

// Record implementation
Record::Record() : blockedAt(0), interval(0) {}

Record::Record(const std::string& fqdn, const std::string& keywordId,
               const std::string& ruleName, const std::vector<std::string>& ips, int interval)
    : fqdn(fqdn), keywordId(keywordId), ruleName(ruleName),
      blockedAt(std::time(nullptr)), lastResolvedIPs(ips), interval(interval) {}

// AuditLogger implementation
void AuditLogger::Initialize(const std::string& auditPath) {
    auditStorePath = auditPath;
}

bool AuditLogger::AddRecord(const Record& record) {
    std::lock_guard<std::mutex> lock(auditMutex);

    try {
        auto records = LoadFromFile();

        // Check if record already exists
        auto it = std::find_if(records.begin(), records.end(),
            [&record](const Record& r) { return r.fqdn == record.fqdn; });

        if (it != records.end()) {
            std::cerr << "Record for FQDN '" << record.fqdn << "' already exists" << std::endl;
            return false;
        }

        records.push_back(record);
        bool success = SaveToFile(records);

        if (success) {
            std::ostringstream oss;
            oss << "Added record for FQDN: " << record.fqdn 
                << " with " << record.lastResolvedIPs.size() << " IP(s)";
            LogAction(oss.str());
        }

        return success;
    }
    catch (const std::exception& e) {
        std::cerr << "Error adding record: " << e.what() << std::endl;
        return false;
    }
}

bool AuditLogger::UpdateRecord(const std::string& fqdn, const std::vector<std::string>& newIPs) {
    std::lock_guard<std::mutex> lock(auditMutex);

    try {
        auto records = LoadFromFile();

        auto it = std::find_if(records.begin(), records.end(),
            [&fqdn](const Record& r) { return r.fqdn == fqdn; });

        if (it == records.end()) {
            std::cerr << "Record not found for FQDN: " << fqdn << std::endl;
            return false;
        }

        it->lastResolvedIPs = newIPs;
        bool success = SaveToFile(records);

        if (success) {
            std::ostringstream oss;
            oss << "Updated record for FQDN: " << fqdn 
                << " with " << newIPs.size() << " IP(s)";
            LogAction(oss.str());
        }

        return success;
    }
    catch (const std::exception& e) {
        std::cerr << "Error updating record: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Record> AuditLogger::ListRecords() {
    std::lock_guard<std::mutex> lock(auditMutex);
    return LoadFromFile();
}

bool AuditLogger::GetRecord(const std::string& fqdn, Record& record) {
    std::lock_guard<std::mutex> lock(auditMutex);

    try {
        auto records = LoadFromFile();

        auto it = std::find_if(records.begin(), records.end(),
            [&fqdn](const Record& r) { return r.fqdn == fqdn; });

        if (it != records.end()) {
            record = *it;
            return true;
        }

        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error getting record: " << e.what() << std::endl;
        return false;
    }
}

bool AuditLogger::RemoveRecord(const std::string& fqdn) {
    std::lock_guard<std::mutex> lock(auditMutex);

    try {
        auto records = LoadFromFile();

        auto it = std::remove_if(records.begin(), records.end(),
            [&fqdn](const Record& r) { return r.fqdn == fqdn; });

        if (it == records.end()) {
            std::cerr << "Record not found for FQDN: " << fqdn << std::endl;
            return false;
        }

        records.erase(it, records.end());
        bool success = SaveToFile(records);

        if (success) {
            std::ostringstream oss;
            oss << "Removed record for FQDN: " << fqdn;
            LogAction(oss.str());
        }

        return success;
    }
    catch (const std::exception& e) {
        std::cerr << "Error removing record: " << e.what() << std::endl;
        return false;
    }
}

void AuditLogger::LogAction(const std::string& message) {
    try {
        std::ofstream logFile(logFilePath, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file: " << logFilePath << std::endl;
            return;
        }

        // Get current time
        std::time_t now = std::time(nullptr);
        std::tm tm;
        localtime_s(&tm, &now);

        logFile << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " - " << message << std::endl;
        logFile.close();
    }
    catch (const std::exception& e) {
        std::cerr << "Error logging action: " << e.what() << std::endl;
    }
}

std::vector<Record> AuditLogger::LoadFromFile() {
    std::vector<Record> records;

    try {
        std::ifstream file(auditStorePath);
        if (!file.is_open()) {
            // File doesn't exist yet, return empty vector
            return records;
        }

        json j;
        file >> j;
        file.close();

        if (j.is_array()) {
            for (const auto& item : j) {
                Record record;
                record.fqdn = item["fqdn"];
                record.keywordId = item["keywordId"];
                record.ruleName = item["ruleName"];
                record.blockedAt = item["blockedAt"];
                record.interval = item["interval"];

                if (item.contains("lastResolvedIPs") && item["lastResolvedIPs"].is_array()) {
                    for (const auto& ip : item["lastResolvedIPs"]) {
                        record.lastResolvedIPs.push_back(ip);
                    }
                }

                records.push_back(record);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading from file: " << e.what() << std::endl;
    }

    return records;
}

bool AuditLogger::SaveToFile(const std::vector<Record>& records) {
    try {
        json j = json::array();

        for (const auto& record : records) {
            json item;
            item["fqdn"] = record.fqdn;
            item["keywordId"] = record.keywordId;
            item["ruleName"] = record.ruleName;
            item["blockedAt"] = record.blockedAt;
            item["interval"] = record.interval;
            item["lastResolvedIPs"] = record.lastResolvedIPs;

            j.push_back(item);
        }

        std::ofstream file(auditStorePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open audit store for writing: " << auditStorePath << std::endl;
            return false;
        }

        file << j.dump(4);  // Pretty print
        file.close();

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving to file: " << e.what() << std::endl;
        return false;
    }
}
