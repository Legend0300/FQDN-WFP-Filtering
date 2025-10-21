#include "Resolver.h"
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

std::vector<std::string> Resolver::ResolveFqdn(const std::string& fqdn) {
    std::vector<std::string> ipAddresses;

    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return ipAddresses;
    }

    // Setup hints for getaddrinfo
    struct addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;    // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo* addrResult = nullptr;

    // Resolve the domain name
    result = getaddrinfo(fqdn.c_str(), nullptr, &hints, &addrResult);
    if (result != 0) {
        std::cerr << "getaddrinfo failed for '" << fqdn << "': " << gai_strerror(result) << std::endl;
        WSACleanup();
        return ipAddresses;
    }

    // Iterate through all resolved addresses
    for (struct addrinfo* ptr = addrResult; ptr != nullptr; ptr = ptr->ai_next) {
        std::string ipAddress;

        if (ptr->ai_family == AF_INET) {
            // IPv4 address
            struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
            ipAddress = IPv4ToString(&(sockaddr_ipv4->sin_addr));
        }
        else if (ptr->ai_family == AF_INET6) {
            // IPv6 address
            struct sockaddr_in6* sockaddr_ipv6 = (struct sockaddr_in6*)ptr->ai_addr;
            ipAddress = IPv6ToString(&(sockaddr_ipv6->sin6_addr));
        }

        if (!ipAddress.empty()) {
            ipAddresses.push_back(ipAddress);
        }
    }

    // Free the address info
    freeaddrinfo(addrResult);
    WSACleanup();

    if (ipAddresses.empty()) {
        std::cerr << "No IP addresses found for FQDN: " << fqdn << std::endl;
    }
    else {
        std::cout << "Resolved " << fqdn << " to " << ipAddresses.size() << " address(es)" << std::endl;
    }

    return ipAddresses;
}

bool Resolver::IsAvailable() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result == 0) {
        WSACleanup();
        return true;
    }
    return false;
}

std::string Resolver::IPv4ToString(const void* addr) {
    char ipStr[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, addr, ipStr, INET_ADDRSTRLEN) != nullptr) {
        return std::string(ipStr);
    }
    return "";
}

std::string Resolver::IPv6ToString(const void* addr) {
    char ipStr[INET6_ADDRSTRLEN];
    if (inet_ntop(AF_INET6, addr, ipStr, INET6_ADDRSTRLEN) != nullptr) {
        return std::string(ipStr);
    }
    return "";
}
