cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Compiler)

file(GLOB SOURCES "src/*.cpp")

add_executable(tier ${SOURCES})
target_include_directories(tier PRIVATE include)
