cmake_minimum_required(VERSION 3.16)
project(astonia3-fan-server C)

# Put build artifacts in centralized folder
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Build options
option(BUILD_CLIENT "Build client executable" OFF)
option(BUILD_SERVER "Build server executables" ON)
option(ENABLE_WARNINGS "Enable extra compile warnings" ON)
option(USE_ARGON2 "Use system argon2 library for password hashing" ON)

if (USE_ARGON2)
    find_path(ARGON2_INCLUDE_DIR argon2.h)
    find_library(ARGON2_LIBRARY argon2)
    if(NOT ARGON2_INCLUDE_DIR OR NOT ARGON2_LIBRARY)
        message(WARNING "USE_ARGON2 requested but argon2 not found; fallback to built-in insecure placeholder.")
        set(USE_ARGON2 OFF)
    else()
        message(STATUS "Argon2 support enabled (library found)")
        include_directories(${ARGON2_INCLUDE_DIR})
    endif()
endif()

if(ENABLE_WARNINGS)
    add_compile_options(
        -Wall -Wextra -Wpedantic -Werror -Wshadow -Wformat-security -Wcast-align
    )
endif()

# C standard and string size
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source code discovery
file(GLOB_RECURSE ASTONIA_SRC
    "*.c"
)

# Exclude bad files if needed
list(FILTER ASTONIA_SRC EXCLUDE REGEX "\\.build\\.|\\.test\\.")

# Build main server line; this includes core C server binaries.
if(BUILD_SERVER)
    add_executable(astonia3-server
        ${ASTONIA_SRC}
    )
    target_compile_definitions(astonia3-server PRIVATE -DDATABASE_DISABLED=0)
    target_include_directories(astonia3-server PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(astonia3-server PRIVATE mysqlclient)
    if(USE_ARGON2)
        target_link_libraries(astonia3-server PRIVATE ${ARGON2_LIBRARY})
        target_compile_definitions(astonia3-server PRIVATE -DUSE_ARGON2=1)
    else()
        target_compile_definitions(astonia3-server PRIVATE -DUSE_ARGON2=0)
    endif()
endif()

# Nuke extra binaries from CMake run; individual project-specific targets can be added here.

# Install instructions
install(TARGETS astonia3-server
    RUNTIME DESTINATION bin
)

message(STATUS "Astonia3 fan server CMake project loaded.")
message(STATUS "Total source files: ${ASTONIA_SRC}")
