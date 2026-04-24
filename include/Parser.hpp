#pragma once

#include "AST.hpp"
#include <Options.hpp>

static Token NULL_TOKEN = {0, 0, "", "", TokenType::NULL_};

bool is_operator(TokenType type);

bool is_type(TokenType type);

void print_expr(const ExprNode& node, int indent = 0);

ExprNode Parse_expression(const std::vector<Token>& expr);

class Parser{
    private:
        std::vector<Token> tokens;
        int pos = 0;
        std::string file_name;

    public:
        Parser(std::vector<Token> u_tokens, std::string u_file_name) { tokens = u_tokens; file_name = u_file_name; }

        const char* get_file_name(){ return file_name.c_str(); }

        Token& at(size_t user_pos){
            if(user_pos >= (int)tokens.size()) return NULL_TOKEN;
            return tokens[user_pos]; 
        }
        Token& current(){ 
            if(tokens.empty()) return NULL_TOKEN;
            return tokens[pos]; 
        }
        Token& peek(size_t offset = 1){ 
            if(pos + offset >= (int)tokens.size()) return NULL_TOKEN;
            return tokens[pos + offset]; 
        }
        Token& consume(size_t offset = 1){ 
            pos += offset;
            if(pos >= (int)tokens.size()) return NULL_TOKEN;
            return tokens[pos]; 
        }
        size_t size(){
            return tokens.size();
        }
        bool check(TokenType token, size_t offset = 1){ return (token == peek(offset).type) ? true : false; }
        std::vector<Token> left(){ return std::vector<Token>(tokens.begin() + pos + 1, tokens.end()); }
        std::vector<Token>& line(){ return tokens; }
};

Parser_AST let_dec(Parser& parser);
Parser_AST set_and_enforce_statements(Parser& parser);
Parser_AST print_statement(Parser& parser);

Parser_AST parser_AST(Parser& parser);