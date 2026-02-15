cmake_minimum_required(VERSION 3.16)

project(MedorCoin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Sources
file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(medorcoin ${SOURCES})

target_include_directories(medorcoin PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

# pthread (needed for Linux / CI)
find_package(Threads REQUIRED)
target_link_libraries(medorcoin PRIVATE Threads::Threads)
