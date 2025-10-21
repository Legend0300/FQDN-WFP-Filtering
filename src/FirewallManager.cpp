#include "FirewallManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fwpmu.h>
#include <rpc.h>

#pragma comment(lib, "fwpuclnt.lib")
#pragma comment(lib, "rpcrt4.lib")

// Initialize static members
HANDLE FirewallManager::engineHandle = nullptr;
bool FirewallManager::initialized = false;

bool FirewallManager::Initialize() {
    if (initialized) {
        return true;
    }

    // Open a session to the filter engine
    DWORD result = FwpmEngineOpen0(
        nullptr,                    // Local machine
        RPC_C_AUTHN_WINNT,         // Authentication service
        nullptr,                    // Authentication identity
        nullptr,                    // Session information
        &engineHandle              // Handle to the engine
    );

    if (result != ERROR_SUCCESS) {
        std::cerr << "FwpmEngineOpen0 failed with error: " << result << std::endl;
        return false;
    }

    initialized = true;
    std::cout << "Firewall Manager initialized successfully" << std::endl;
    return true;
}

void FirewallManager::Cleanup() {
    if (engineHandle != nullptr) {
        FwpmEngineClose0(engineHandle);
        engineHandle = nullptr;
        initialized = false;
        std::cout << "Firewall Manager cleaned up" << std::endl;
    }
}

std::string FirewallManager::CreateDynamicKeywordAddress(const std::string& fqdn,
                                                         const std::vector<std::string>& ips,
                                                         bool autoResolve) {
    if (!initialized) {
        std::cerr << "FirewallManager not initialized" << std::endl;
        return "";
    }

    // Generate a GUID for this keyword address
    std::string guidStr = GenerateGUID();

    std::cout << "Creating dynamic keyword address for: " << fqdn << std::endl;
    std::cout << "GUID: " << guidStr << std::endl;

    // NOTE: Windows Filtering Platform Dynamic Keyword Addresses
    // This is a placeholder implementation. The actual WFP API for dynamic keyword addresses
    // requires using FwpmDynamicKeywordSubscribe0 and related functions which are available
    // in Windows 10 version 2004 and later.
    //
    // Full implementation would use:
    // - FwpmDynamicKeywordSubscribe0() to register the keyword
    // - FwpmDynamicKeywordAddressAdd0() to add IP addresses
    // - Create filters that reference this keyword address
    //
    // For this implementation, we're creating a conceptual wrapper that demonstrates
    // the structure. In production, you would call the actual WFP APIs here.

    // Store the keyword-IP mapping (in a real implementation, this would be in WFP)
    std::cout << "Dynamic keyword address created with " << ips.size() << " IP(s)" << std::endl;
    for (const auto& ip : ips) {
        std::cout << "  - " << ip << std::endl;
    }

    // Return the GUID
    return guidStr;
}

bool FirewallManager::UpdateDynamicKeywordAddress(const std::string& keywordId,
                                                  const std::vector<std::string>& ips) {
    if (!initialized) {
        std::cerr << "FirewallManager not initialized" << std::endl;
        return false;
    }

    std::cout << "Updating dynamic keyword address: " << keywordId << std::endl;
    std::cout << "New IP count: " << ips.size() << std::endl;

    // NOTE: Production implementation would use:
    // - FwpmDynamicKeywordAddressUpdate0() to update the IP list
    // This would replace all IPs associated with the keyword ID

    for (const auto& ip : ips) {
        std::cout << "  - " << ip << std::endl;
    }

    return true;
}

bool FirewallManager::DeleteDynamicKeywordAddress(const std::string& keywordId) {
    if (!initialized) {
        std::cerr << "FirewallManager not initialized" << std::endl;
        return false;
    }

    std::cout << "Deleting dynamic keyword address: " << keywordId << std::endl;

    // NOTE: Production implementation would use:
    // - FwpmDynamicKeywordAddressDelete0() to remove the keyword address

    return true;
}

bool FirewallManager::CreateFirewallRule(const std::string& ruleName,
                                        const std::string& keywordId,
                                        const std::string& direction,
                                        const std::string& action) {
    if (!initialized) {
        std::cerr << "FirewallManager not initialized" << std::endl;
        return false;
    }

    std::cout << "Creating firewall rule: " << ruleName << std::endl;
    std::cout << "  Direction: " << direction << std::endl;
    std::cout << "  Action: " << action << std::endl;
    std::cout << "  Keyword ID: " << keywordId << std::endl;

    // Convert rule name to wide string
    int size = MultiByteToWideChar(CP_UTF8, 0, ruleName.c_str(), -1, nullptr, 0);
    std::wstring wRuleName(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, ruleName.c_str(), -1, &wRuleName[0], size);

    // NOTE: Full WFP implementation would:
    // 1. Create a filter condition that references the dynamic keyword address
    // 2. Set up FWPM_FILTER0 structure with:
    //    - Filter name
    //    - Layer (e.g., FWPM_LAYER_ALE_AUTH_CONNECT_V4)
    //    - Action (FWP_ACTION_BLOCK or FWP_ACTION_PERMIT)
    //    - Filter conditions referencing the keyword
    // 3. Call FwpmFilterAdd0() to add the filter
    //
    // Example structure (pseudo-code):
    /*
    FWPM_FILTER0 filter = {};
    filter.displayData.name = wRuleName.c_str();
    filter.layerKey = (direction == "Outbound") ? 
                      FWPM_LAYER_ALE_AUTH_CONNECT_V4 : 
                      FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4;
    filter.action.type = (action == "Block") ? FWP_ACTION_BLOCK : FWP_ACTION_PERMIT;
    
    FWPM_FILTER_CONDITION0 condition = {};
    condition.fieldKey = FWPM_CONDITION_DYNAMIC_KEYWORD_ADDRESS;
    // Set condition value to reference keywordId
    
    filter.numFilterConditions = 1;
    filter.filterCondition = &condition;
    
    UINT64 filterId;
    DWORD result = FwpmFilterAdd0(engineHandle, &filter, nullptr, &filterId);
    */

    std::cout << "Firewall rule created successfully (simulation)" << std::endl;
    return true;
}

bool FirewallManager::DeleteFirewallRule(const std::string& ruleName) {
    if (!initialized) {
        std::cerr << "FirewallManager not initialized" << std::endl;
        return false;
    }

    std::cout << "Deleting firewall rule: " << ruleName << std::endl;

    // NOTE: Production implementation would:
    // 1. Enumerate filters to find the one with matching name
    // 2. Call FwpmFilterDeleteByKey0() or FwpmFilterDeleteById0()
    //
    // Example:
    /*
    HANDLE enumHandle = nullptr;
    FWPM_FILTER_ENUM_TEMPLATE0 enumTemplate = {};
    
    DWORD result = FwpmFilterCreateEnumHandle0(engineHandle, &enumTemplate, &enumHandle);
    if (result == ERROR_SUCCESS) {
        FWPM_FILTER0** filters = nullptr;
        UINT32 numFilters = 0;
        
        FwpmFilterEnum0(engineHandle, enumHandle, 100, &filters, &numFilters);
        
        for (UINT32 i = 0; i < numFilters; i++) {
            if (wcscmp(filters[i]->displayData.name, wRuleName.c_str()) == 0) {
                FwpmFilterDeleteById0(engineHandle, filters[i]->filterId);
                break;
            }
        }
        
        FwpmFilterDestroyEnumHandle0(engineHandle, enumHandle);
    }
    */

    std::cout << "Firewall rule deleted successfully (simulation)" << std::endl;
    return true;
}

std::string FirewallManager::GenerateGUID() {
    GUID guid;
    HRESULT hr = CoCreateGuid(&guid);
    
    if (FAILED(hr)) {
        std::cerr << "Failed to generate GUID" << std::endl;
        return "";
    }

    // Convert GUID to string
    std::ostringstream oss;
    oss << std::hex << std::setfill('0')
        << std::setw(8) << guid.Data1 << "-"
        << std::setw(4) << guid.Data2 << "-"
        << std::setw(4) << guid.Data3 << "-"
        << std::setw(2) << static_cast<int>(guid.Data4[0])
        << std::setw(2) << static_cast<int>(guid.Data4[1]) << "-"
        << std::setw(2) << static_cast<int>(guid.Data4[2])
        << std::setw(2) << static_cast<int>(guid.Data4[3])
        << std::setw(2) << static_cast<int>(guid.Data4[4])
        << std::setw(2) << static_cast<int>(guid.Data4[5])
        << std::setw(2) << static_cast<int>(guid.Data4[6])
        << std::setw(2) << static_cast<int>(guid.Data4[7]);

    return oss.str();
}

std::vector<BYTE> FirewallManager::IPStringToBytes(const std::string& ipStr, bool& isIPv6) {
    std::vector<BYTE> bytes;
    
    // Try IPv4 first
    struct in_addr addr4;
    if (inet_pton(AF_INET, ipStr.c_str(), &addr4) == 1) {
        isIPv6 = false;
        BYTE* ptr = reinterpret_cast<BYTE*>(&addr4);
        bytes.assign(ptr, ptr + sizeof(in_addr));
        return bytes;
    }

    // Try IPv6
    struct in6_addr addr6;
    if (inet_pton(AF_INET6, ipStr.c_str(), &addr6) == 1) {
        isIPv6 = true;
        BYTE* ptr = reinterpret_cast<BYTE*>(&addr6);
        bytes.assign(ptr, ptr + sizeof(in6_addr));
        return bytes;
    }

    // Invalid IP address
    return bytes;
}
