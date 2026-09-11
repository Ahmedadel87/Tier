cmake_minimum_required(VERSION 3.16)

project(Tier LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    main.cpp

    Lexer/src/Lexer.cpp

    Support/SourceManager/SourceManager.cpp

    Parser/src/Parser.cpp
    Parser/src/ParserDiag.cpp

    Diagnostic/src/DiagnosticBuilder.cpp
    Diagnostic/src/DiagnosticEngine.cpp
    Diagnostic/src/DiagnosticRenderer.cpp
)

set(HEADERS
    AST/AST.hpp

    Basic/Diagnostic.hpp
    Basic/Diagnostics.hpp

    Basic/Token.hpp

    Lexer/include/Lexer.hpp

    Support/SourceManager/SourceManager.hpp

    Support/MemoryManager/MemoryManager.hpp

    Parser/include/Parser.hpp

    Diagnostic/include/DiagnosticBuilder.hpp
    Diagnostic/include/DiagnosticEngine.hpp
    Diagnostic/include/DiagnosticRenderer.hpp
)

add_executable(tier
    ${SOURCES}
    ${HEADERS}
)

target_compile_features(tier PRIVATE cxx_std_23)

target_compile_options(tier PRIVATE
    -fsanitize=address
    -fno-omit-frame-pointer
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CONFIG:Debug>:-O0 -g>
)

target_link_options(tier PRIVATE
    -fsanitize=address
)

target_include_directories(tier PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/AST
    ${CMAKE_CURRENT_SOURCE_DIR}/Basic
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Support/SourceManager
    ${CMAKE_CURRENT_SOURCE_DIR}/Support/MemoryManager
    ${CMAKE_CURRENT_SOURCE_DIR}/Parser/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Diagnostic/include
)