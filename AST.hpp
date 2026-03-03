#pragma once

#include "Lexer.hpp"
#include <variant>

/*
DECLARATION, ASSIGNMENT, PRINT
*/
enum class AST1_Type{
    DECLARATION,
    ASSIGNMENT,
    PRINT,
    NULL_
};

struct Declaration_Node{
    std::string identifier;
    TokenType type; //* Only TokenTypes that are types will be assigned here, ex: i32
    std::vector<Token> expr;
};

using Node = std::variant<Declaration_Node>;

struct AST1_NODE{
    AST1_Type type;
    Node node;
    bool iterable;
    bool const_;
    const std::vector<Token>* reference;
};