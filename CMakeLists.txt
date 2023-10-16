cmake_minimum_required(VERSION 3.27)
project("LanguageC")


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMKAE_EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set (SRC
    src/main.cpp
    src/lexer.cpp
    src/lexer.hpp
    src/parser.cpp
    src/parser.hpp
    src/ast.cpp
    src/ast.hpp
    src/astGen.cpp
    src/astGen.hpp
    src/type_checker.hpp
    src/XIR.hpp
    src/XIR.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
