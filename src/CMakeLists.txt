cmake_minimum_required(VERSION 3.16)

project(MedorCoinNode LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCE_FILES
    main.cpp
    blockchain.cpp
    transaction.cpp
    utxo.cpp
    crypto/keystore.cpp
    crypto/evm_sign.cpp
    crypto/rlp.cpp
    crypto/signature.cpp
    net/net_manager.cpp
    net/sync_manager.cpp
    net/serialization.cpp
    blockchain_fork.cpp
)

add_executable(MedorCoinNode ${SOURCE_FILES})

# Include paths
target_include_directories(MedorCoinNode
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/net
        ${CMAKE_CURRENT_SOURCE_DIR}/crypto
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# pkg-config required
find_package(PkgConfig REQUIRED)

# LevelDB via pkg-config
pkg_check_modules(LEVELDB REQUIRED leveldb)

target_include_directories(MedorCoinNode
    PRIVATE
        ${LEVELDB_INCLUDE_DIRS}
)

target_link_libraries(MedorCoinNode
    PRIVATE
        ${LEVELDB_LIBRARIES}
)

# OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(MedorCoinNode
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
)

# secp256k1 via pkg-config
pkg_check_modules(SECP256K1 REQUIRED libsecp256k1)

target_include_directories(MedorCoinNode
    PRIVATE
        ${SECP256K1_INCLUDE_DIRS}
)

target_link_libraries(MedorCoinNode
    PRIVATE
        ${SECP256K1_LIBRARIES}
)

# POSIX networking support
if (UNIX)
    target_link_libraries(MedorCoinNode PRIVATE pthread)
endif()
