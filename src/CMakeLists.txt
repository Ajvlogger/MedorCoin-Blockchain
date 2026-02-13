# Build executable for MedorCoin node

# All source files
set(SOURCES
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

# Add the executable
add_executable(MedorCoinNode ${SOURCES})

# Public include directories for this target
target_include_directories(MedorCoinNode
    PUBLIC
        ${PROJECT_SOURCE_DIR}/../include
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/net
        ${PROJECT_SOURCE_DIR}/crypto
)

# Link libraries if needed (example: leveldb, secp256k1, evmc)
find_package(LevelDB REQUIRED)
find_library(SECP256K1 secp256k1)
find_library(EVMC evmc)  # adjust as needed

target_link_libraries(MedorCoinNode
    PRIVATE
        LevelDB::LevelDB
        ${SECP256K1}
        ${EVMC}
)
