cmake_minimum_required(VERSION 3.16)

project(CompilerProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    main.cpp

    Lexer/src/Lexer.cpp

    Support/SourceManager/SourceManager.cpp

    Diagnostic/Diagnostic.cpp

    Parser/src/Parser.cpp
    Parser/src/ParserDiag.cpp

    Basic/Token.cpp
)

set(HEADERS
    AST/AST.hpp

    Basic/Diagnostic.hpp
    Basic/Token.hpp

    Lexer/include/Lexer.hpp

    Support/SourceManager/SourceManager.hpp

    Support/MemoryManager/MemoryManager.hpp

    Parser/include/Parser.hpp
)

add_executable(compiler
    ${SOURCES}
    ${HEADERS}
)

target_compile_features(compiler PRIVATE cxx_std_23)

target_compile_options(compiler PRIVATE
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CONFIG:Debug>:-O0 -g>
)

target_include_directories(compiler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/AST
    ${CMAKE_CURRENT_SOURCE_DIR}/Basic
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Support/SourceManager
    ${CMAKE_CURRENT_SOURCE_DIR}/Support/MemoryManager
    ${CMAKE_CURRENT_SOURCE_DIR}/Parser/include
)