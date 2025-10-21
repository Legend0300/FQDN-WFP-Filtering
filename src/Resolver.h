#ifndef RESOLVER_H
#define RESOLVER_H

#include <string>
#include <vector>

/**
 * @brief DNS Resolution utilities
 * 
 * Provides DNS resolution functionality to convert FQDNs to IP addresses.
 * Supports both IPv4 and IPv6.
 */
class Resolver {
public:
    /**
     * @brief Resolve an FQDN to a list of IP addresses
     * @param fqdn Fully Qualified Domain Name to resolve
     * @return Vector of IP addresses (IPv4 and IPv6) as strings
     */
    static std::vector<std::string> ResolveFqdn(const std::string& fqdn);

    /**
     * @brief Check if DNS resolution is available
     * @return true if resolution can be performed, false otherwise
     */
    static bool IsAvailable();

private:
    /**
     * @brief Convert IPv4 address to string
     * @param addr Pointer to sockaddr_in structure
     * @return IP address as string
     */
    static std::string IPv4ToString(const void* addr);

    /**
     * @brief Convert IPv6 address to string
     * @param addr Pointer to sockaddr_in6 structure
     * @return IP address as string
     */
    static std::string IPv6ToString(const void* addr);
};

#endif // RESOLVER_H
