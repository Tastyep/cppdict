cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "cppdict")
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_COMPILER clang++)

set(SOURCE_DIR src)
set(INCLUDE_DIR include)

file(GLOB_RECURSE PROJECT_SOURCE_FILES
     "${SOURCE_DIR}/*pp")

include_directories(${INCLUDE_DIR})
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})

