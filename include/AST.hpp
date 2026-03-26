#pragma once

#include "Lexer.hpp"
#include <variant>
#include <memory>

/*
DECLARATION, ASSIGNMENT, PRINT
*/
enum class Parser_AST_Type{
    DECLARATION,
    ASSIGNMENT,
    PRINT,
    TYPE_GROUP_DEC,
    NULL_
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct LiteralExpr { Token value; };

struct ExprNode;

struct BinaryExpr {
    Token op;
    std::unique_ptr<ExprNode> left;
    std::unique_ptr<ExprNode> right;
};

struct ExprNode {
    std::variant<LiteralExpr, BinaryExpr> node;
};

struct Declaration_Node{
    std::string identifier;
    TokenType type; //* Only TokenTypes that are types will be assigned here, ex: i32
    ExprNode expr;
    bool const_;
};

struct Type_group_Declaration_Node{
    std::string typegroup;
    std::vector<TokenType> types; //* Only TokenTypes that are types will be assigned here, ex: i32
};

struct Type_group_infer_instead{
    std::string typegroup;
    std::vector<TokenType> types; //* Only TokenTypes that are types will be assigned here, ex: i32
};

using Parser_Node = std::variant<Declaration_Node, Type_group_Declaration_Node>;

struct Parser_AST{
    Parser_AST_Type type;
    Parser_Node node;
    const std::vector<Token> reference;
};