cmake_minimum_required(VERSION 3.16)

project(MedorCoinNode LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect all source files
file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(medorcoin ${SOURCE_FILES})

# Include directories for the executable
target_include_directories(medorcoin
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/net
        ${CMAKE_CURRENT_SOURCE_DIR}/crypto
        ${PROJECT_SOURCE_DIR}/../include
)

# OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(medorcoin
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
)

# secp256k1 (optional - if your project uses it via pkg-config)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SECP256K1 REQUIRED libsecp256k1)

target_include_directories(medorcoin
    PRIVATE
        ${SECP256K1_INCLUDE_DIRS}
)

target_link_libraries(medorcoin
    PRIVATE
        ${SECP256K1_LIBRARIES}
)

# Threads (for networking)
find_package(Threads REQUIRED)
target_link_libraries(medorcoin
    PRIVATE
        Threads::Threads
)
