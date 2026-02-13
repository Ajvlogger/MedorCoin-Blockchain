cmake_minimum_required(VERSION 3.20)

# Project metadata
project(MedorCoin
    VERSION 0.1
    DESCRIPTION "MedorCoin Blockchain"
    LANGUAGES CXX
)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable coverage flags when requested
option(ENABLE_COVERAGE "Enable code coverage" OFF)
if(ENABLE_COVERAGE)
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

# Include directories (for headers)
include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

# Add subdirectories
add_subdirectory(src)

# Optional: add tests if present
if(EXISTS "${PROJECT_SOURCE_DIR}/tests")
    enable_testing()
    add_subdirectory(tests)
endif()
