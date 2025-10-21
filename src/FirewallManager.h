#ifndef FIREWALLMANAGER_H
#define FIREWALLMANAGER_H

#include <string>
#include <vector>
#include <Windows.h>

/**
 * @brief Windows Firewall Platform (WFP) management
 * 
 * Provides wrapper functions for Windows Filtering Platform APIs,
 * specifically for managing dynamic keyword addresses and firewall rules.
 * 
 * Dynamic Keyword Addresses allow creating named sets of IP addresses that
 * can be updated dynamically and referenced in firewall rules.
 */
class FirewallManager {
public:
    /**
     * @brief Initialize the Firewall Manager
     * @return true if initialization successful, false otherwise
     */
    static bool Initialize();

    /**
     * @brief Cleanup and close firewall handles
     */
    static void Cleanup();

    /**
     * @brief Create a dynamic keyword address object
     * 
     * Creates a named dynamic keyword address that can be populated with IP addresses
     * and referenced in firewall rules.
     * 
     * @param fqdn FQDN to use as the keyword name
     * @param ips Initial IP addresses to add
     * @param autoResolve Enable auto-resolution (currently not used, for future extension)
     * @return GUID of the created keyword address, or empty string on failure
     */
    static std::string CreateDynamicKeywordAddress(const std::string& fqdn,
                                                   const std::vector<std::string>& ips,
                                                   bool autoResolve = true);

    /**
     * @brief Update a dynamic keyword address with new IP addresses
     * 
     * Replaces the IP addresses associated with a dynamic keyword address.
     * 
     * @param keywordId GUID of the keyword address
     * @param ips New IP addresses to set
     * @return true if successful, false otherwise
     */
    static bool UpdateDynamicKeywordAddress(const std::string& keywordId,
                                           const std::vector<std::string>& ips);

    /**
     * @brief Delete a dynamic keyword address
     * @param keywordId GUID of the keyword address to delete
     * @return true if successful, false otherwise
     */
    static bool DeleteDynamicKeywordAddress(const std::string& keywordId);

    /**
     * @brief Create a firewall rule that blocks traffic to/from a keyword address
     * 
     * @param ruleName Name of the firewall rule
     * @param keywordId GUID of the dynamic keyword address
     * @param direction "Outbound" or "Inbound"
     * @param action "Block" or "Allow"
     * @return true if successful, false otherwise
     */
    static bool CreateFirewallRule(const std::string& ruleName,
                                   const std::string& keywordId,
                                   const std::string& direction,
                                   const std::string& action);

    /**
     * @brief Delete a firewall rule by name
     * @param ruleName Name of the rule to delete
     * @return true if successful, false otherwise
     */
    static bool DeleteFirewallRule(const std::string& ruleName);

    /**
     * @brief Generate a GUID string
     * @return GUID as string
     */
    static std::string GenerateGUID();

private:
    static HANDLE engineHandle;  // Handle to WFP engine
    static bool initialized;

    /**
     * @brief Convert IP address string to byte array
     * @param ipStr IP address string
     * @param isIPv6 Output parameter indicating if IPv6
     * @return Byte array representation
     */
    static std::vector<BYTE> IPStringToBytes(const std::string& ipStr, bool& isIPv6);
};

#endif // FIREWALLMANAGER_H
