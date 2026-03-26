#pragma once

#include "AST.hpp"

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

        Token& current(){ 
            if(tokens.empty()) return NULL_TOKEN;
            return tokens[pos]; 
        }
        Token& peek(int offset = 1){ 
            if(pos + offset >= (int)tokens.size()) return NULL_TOKEN;
            return tokens[pos + offset]; 
        }
        Token& consume(int offset = 1){ 
            pos += offset;
            if(pos >= (int)tokens.size()) return NULL_TOKEN;
            return tokens[pos]; 
        }
        bool check(TokenType token, int offset = 1){ return (token == peek(offset).type) ? true : false; }
        std::vector<Token> left(){ return std::vector<Token>(tokens.begin() + pos + 1, tokens.end()); }
        std::vector<Token>& line(){ return tokens; }
};

Parser_AST let_dec(Parser& parser);

Parser_AST parser_AST(Parser& parser);