cmake_minimum_required(VERSION 3.15)
project(FqdnBlockerCli VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/lib)

# Source files
set(SOURCES
    src/main.cpp
    src/Config.cpp
    src/AuditLogger.cpp
    src/FirewallManager.cpp
    src/Resolver.cpp
    src/Scheduler.cpp
)

# Header files
set(HEADERS
    src/Config.h
    src/AuditLogger.h
    src/FirewallManager.h
    src/Resolver.h
    src/Scheduler.h
)

# Create executable
add_executable(FqdnBlockerCli ${SOURCES} ${HEADERS})

# Link Windows libraries
target_link_libraries(FqdnBlockerCli
    ws2_32          # Winsock
    iphlpapi        # IP Helper API
    fwpuclnt        # Windows Filtering Platform User-mode API
    rpcrt4          # RPC Runtime (for GUID operations)
    advapi32        # Advanced Windows API
)

# Set output directories
set_target_properties(FqdnBlockerCli PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build
)

# Request Administrator privileges
if(MSVC)
    set_target_properties(FqdnBlockerCli PROPERTIES
        LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
    )
endif()

# Copy config files to build directory
add_custom_command(TARGET FqdnBlockerCli POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/config ${CMAKE_SOURCE_DIR}/build/config
)

# Create data and logs directories in build
add_custom_command(TARGET FqdnBlockerCli POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build/data
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build/logs
)
