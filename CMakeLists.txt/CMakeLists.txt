cmake_minimum_required(VERSION 3.10)

project(MedorCoin VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include your headers
target_include_directories(${PROJECT_NAME} BEFORE PRIVATE "${PROJECT_SOURCE_DIR}/include")

# Find LevelDB library
find_library(LEVELDB_LIB leveldb)

# List all source files
set(SOURCE_FILES
    src/main.cpp
    src/consensus.cpp
    src/crypto.cpp
    src/transaction.cpp
    src/mempool.cpp
    src/blockchain.cpp
    src/db/blockdb.cpp
    src/net/peer_manager.cpp
    src/net/net_manager.cpp
)

# Define the executable
add_executable(MedorCoin ${SOURCE_FILES})

# Link LevelDB to your executable
target_link_libraries(MedorCoin PRIVATE ${LEVELDB_LIB})
