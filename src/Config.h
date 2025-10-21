#ifndef CONFIG_H
#define CONFIG_H

#include <string>

/**
 * @brief Configuration management for FQDN Blocker CLI
 * 
 * Handles loading and saving application configuration including:
 * - Default refresh interval for DNS resolution
 * - Log file path
 * - Audit store path
 */
class Config {
public:
    /**
     * @brief Load configuration from file
     * @param configPath Path to the configuration file (JSON)
     * @return true if configuration loaded successfully, false otherwise
     */
    static bool Load(const std::string& configPath);

    /**
     * @brief Save current configuration to file
     * @param configPath Path to save the configuration file
     * @return true if configuration saved successfully, false otherwise
     */
    static bool Save(const std::string& configPath);

    /**
     * @brief Get the default refresh interval in minutes
     * @return Default interval in minutes
     */
    static int GetDefaultInterval();

    /**
     * @brief Set the default refresh interval
     * @param intervalMinutes Interval in minutes
     */
    static void SetDefaultInterval(int intervalMinutes);

    /**
     * @brief Get the log file path
     * @return Path to log file
     */
    static std::string GetLogFilePath();

    /**
     * @brief Set the log file path
     * @param path Path to log file
     */
    static void SetLogFilePath(const std::string& path);

    /**
     * @brief Get the audit store path
     * @return Path to audit store file
     */
    static std::string GetAuditStorePath();

    /**
     * @brief Set the audit store path
     * @param path Path to audit store file
     */
    static void SetAuditStorePath(const std::string& path);

private:
    static int defaultInterval;           // in minutes
    static std::string logFilePath;
    static std::string auditStorePath;
};

#endif // CONFIG_H
