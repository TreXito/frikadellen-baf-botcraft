cmake_minimum_required(VERSION 3.15)

project(frikadellen-baf-botcraft VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BOTCRAFT_USE_SYSTEM "Use system-installed Botcraft instead of FetchContent" OFF)
option(BUILD_WITH_BOTCRAFT "Build with full Botcraft integration (requires Botcraft)" ON)

# Find dependencies
find_package(Threads REQUIRED)

# Botcraft integration (optional for build demonstration)
if(BUILD_WITH_BOTCRAFT)
    if(BOTCRAFT_USE_SYSTEM)
        find_package(botcraft REQUIRED)
    else()
        # FetchContent for Botcraft can be added here
        # For now, assume system install
        find_package(botcraft QUIET)
        if(NOT botcraft_FOUND)
            message(WARNING "Botcraft not found. Building without Botcraft integration.")
            message(WARNING "Set BUILD_WITH_BOTCRAFT=OFF or install Botcraft to build with full functionality.")
            set(BUILD_WITH_BOTCRAFT OFF)
        endif()
    endif()
endif()

# Check for nlohmann_json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    # Try to use header-only version from system
    find_path(NLOHMANN_JSON_INCLUDE_DIR
        NAMES nlohmann/json.hpp
        PATHS /usr/include /usr/local/include
    )
    if(NLOHMANN_JSON_INCLUDE_DIR)
        add_library(nlohmann_json INTERFACE)
        target_include_directories(nlohmann_json INTERFACE ${NLOHMANN_JSON_INCLUDE_DIR})
    else()
        message(WARNING "nlohmann_json not found. Some features may not compile.")
    endif()
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/config.cpp
    src/logger.cpp
    src/cofl_client.cpp
    src/bazaar_handler.cpp
    src/auction_handler.cpp
    src/webhook.cpp
    src/utils.cpp
)

# Create executable
add_executable(frikadellen-baf ${SOURCES})

# Include directories
target_include_directories(frikadellen-baf
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(frikadellen-baf
    PRIVATE
        Threads::Threads
)

# Conditional Botcraft linking
if(BUILD_WITH_BOTCRAFT AND botcraft_FOUND)
    target_link_libraries(frikadellen-baf PRIVATE botcraft::botcraft)
    target_compile_definitions(frikadellen-baf PRIVATE WITH_BOTCRAFT)
    message(STATUS "Building with Botcraft support")
else()
    message(STATUS "Building without Botcraft (demonstration mode only)")
endif()

# Link nlohmann_json if available
if(TARGET nlohmann_json)
    target_link_libraries(frikadellen-baf PRIVATE nlohmann_json)
endif()

# Installation
install(TARGETS frikadellen-baf
    RUNTIME DESTINATION bin
)

# Copy config template
install(FILES config.toml.example
    DESTINATION bin
    RENAME config.toml
)

# Print build configuration
message(STATUS "=== Frikadellen BAF Build Configuration ===")
message(STATUS "Build with Botcraft: ${BUILD_WITH_BOTCRAFT}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "==========================================")

