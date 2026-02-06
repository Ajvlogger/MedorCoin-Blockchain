cmake_minimum_required(VERSION 3.10)

project(MedorCoin VERSION 1.0 LANGUAGES CXX)

# Tell CMake to compile using C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directory for your headers
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/db
    ${PROJECT_SOURCE_DIR}/include/net
)

# Find LevelDB installed on the system
find_library(LEVELDB_LIB leveldb)

# List all your source code files
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

# Create the executable target
add_executable(medorcoin ${SOURCE_FILES})

# Link against LevelDB
target_link_libraries(medorcoin PRIVATE ${LEVELDB_LIB})
