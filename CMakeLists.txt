cmake_minimum_required(VERSION 3.10)

project(MedorCoin VERSION 1.0 LANGUAGES C CXX)

# ----------------------------
# C++ Standard
# ----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------
# Include Directories
# ----------------------------
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/db
    ${PROJECT_SOURCE_DIR}/src/abi
    ${PROJECT_SOURCE_DIR}/src/crypto/keccak
    ${PROJECT_SOURCE_DIR}/src/net
)

# ----------------------------
# Find Libraries
# ----------------------------

# Find LevelDB
find_library(LEVELDB_LIB
    NAMES leveldb
)

# Find RocksDB
find_library(ROCKSDB_LIB
    NAMES rocksdb
)

# Link them
target_link_libraries(medorcoin
    PRIVATE
        ${LEVELDB_LIB}
        ${ROCKSDB_LIB}
        evmone
        nlohmann_json::nlohmann_json
)

# RocksDB
find_library(ROCKSDB_LIB
    NAMES rocksdb
)

# JSON library (nlohmann/json)
find_package(nlohmann_json 3.2.0 REQUIRED)

# ----------------------------
# EVM1 Engine
# ----------------------------
add_subdirectory(third_party/evmone)

# ----------------------------
# Source Files
# ----------------------------
set(SOURCE_FILES
    src/main.cpp
    src/consensus.cpp
    src/crypto.cpp
    src/transaction.cpp
    src/mempool.cpp
    src/blockchain.cpp

    # Database wrappers
    src/db/blockdb.cpp
    src/db/accountdb.cpp

    # ABI parsing/encoding
    src/abi/abi.cpp
    src/abi/encode.cpp

    # Keccak C implementation
    src/crypto/keccak/keccak.c

    # Networking
    src/net/peer_manager.cpp
    src/net/net_manager.cpp

    # UTXO
    src/utxo.cpp

    # Wallet
    src/wallet.cpp
)

# ----------------------------
# Build Executable
# ----------------------------
add_executable(medorcoin
    ${SOURCE_FILES}
)

# ----------------------------
# Link Libraries
# ----------------------------
target_link_libraries(medorcoin
    PRIVATE
        ${LEVELDB_LIB}
        ${ROCKSDB_LIB}
        evmone
        nlohmann_json::nlohmann_json
)

# ----------------------------
# Optional Debug Definitions
# ----------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(medorcoin PRIVATE DEBUG)
endif()
