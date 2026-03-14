#pragma once

#include "AST.hpp"

bool is_operator(TokenType type);

bool is_type(TokenType type);

void print_expr(const ExprNode& node, const std::string& prefix = "", bool is_right = false);

ExprNode Parse_expression(const std::vector<Token>& expr);

class Parser{
    private:
        std::vector<Token> tokens;
        int pos = 0;

    public:
        Parser(std::vector<Token> user_tokens) { tokens = user_tokens; }
        Token& current(){ return tokens[pos]; }
        Token& peek(int offset = 1){ return tokens[pos + offset]; }
        Token& consume(int offset = 1){ return tokens[pos += offset]; }
        bool check(TokenType token, int offset = 1){ return (token == peek(offset).type) ? true : false; }
        std::vector<Token> left(){ return std::vector<Token>(tokens.begin() + pos + 1, tokens.end()); }
        std::vector<Token>& line(){ return tokens; }
};

AST_NODE let_dec(Parser parser);

void AST(Parser parser);