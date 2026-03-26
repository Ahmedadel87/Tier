
#include "AST.hpp"
#include "Semantic Analysis.hpp"
#include <variant>
#include <iostream>

static const std::unordered_map<std::string, symbol> symbol_table;

Semantic_AST declaration(Declaration_Node& parser_AST, const int current_scope_id){
    /*
    struct Declaration_Node{
        std::string identifier;
        TokenType type; //* Only TokenTypes that are types will be assigned here, ex: i32
        ExprNode expr;
        bool const_;
    };
    struct symbol{
        AST org_AST;
        int scope_id;
        std::vector<int> parents_id;
    };
    struct DECLARATION{ 
        TokenType type; //* Only TokenTypes that are types will be assigned here, ex: i32
        std::string ident;
        ExprNode expr;
        bool is_mutable;
        bool is_iterable;
    };
    struct AST {
        AST_type type;
        Node node;
    };
    */
   return Semantic_AST{AST_type::DECLARATION, DECLARATION{parser_AST.type, parser_AST.identifier, std::move(parser_AST.expr), false, false}};
}

std::vector<Semantic_AST> Semantic_Parser(std::vector<Parser_AST>& Parser_ASTs){
    for(Parser_AST& parser_AST : Parser_ASTs){
        switch(parser_AST.type){
            case Parser_AST_Type::DECLARATION:
                if(auto* node = std::get_if<Declaration_Node>(&parser_AST.node)){
                    declaration(*node, 0);
                } 
                break;
        }
    }
    std::vector<Semantic_AST> empty;
    return empty;
}