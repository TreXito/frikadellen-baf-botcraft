cmake_minimum_required(VERSION 3.15)

project(frikadellen-baf-botcraft VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Botcraft - version for Minecraft 1.21.11
find_package(botcraft REQUIRED)

# Find other dependencies
find_package(Threads REQUIRED)

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

# Link libraries
target_link_libraries(frikadellen-baf
    PRIVATE
        botcraft::botcraft
        Threads::Threads
)

# Include directories
target_include_directories(frikadellen-baf
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Installation
install(TARGETS frikadellen-baf
    RUNTIME DESTINATION bin
)

# Copy config template
install(FILES config.toml.example
    DESTINATION bin
)
