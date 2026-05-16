
#include "AST.hpp"
#include "Semantic Analysis.hpp"
#include <variant>
#include <iostream>

static const std::unordered_map<std::string, symbol> symbol_table;

Semantic_AST declaration(Declaration_Node& parser_AST, const int current_scope_id){
    // TODO: Symbol Table
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