#pragma once

#include <variant>
#include "Lexer.hpp"
#include "AST.hpp"
#include <Options.hpp>

enum class AST_type{
    DECLARATION,
};

struct DECLARATION{ 
    TokenType type; //* Only TokenTypes that are types will be assigned here, ex: i32
    std::string identifier;
    ExprNode expr;
    bool is_mutable;
    bool is_iterable;
};

using Semantic_Node = std::variant<DECLARATION>;

struct Semantic_AST{
    AST_type type;
    Semantic_Node node;
};

struct symbol{
    Semantic_AST& org_AST;
    int scope_id;
    std::vector<int> parents_id;
};

std::vector<Semantic_AST> Semantic_Parser(std::vector<Parser_AST>& Parser_ASTs);