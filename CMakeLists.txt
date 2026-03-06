cmake_minimum_required(VERSION 3.20)

project(Compiler)

file(GLOB SOURCES "src/*.cpp")

add_executable(compiler ${SOURCES})
target_include_directories(compiler PRIVATE include)
