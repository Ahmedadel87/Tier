#pragma once

#include "AST.hpp"

struct Error{
    int error_code;
    int line;
    std::vector<Token*> error_tokens;
};

enum class Expected{
    IDENTIFIER,
    VARIABLE,
    COLON,
    TYPE, //* i8 -> i64 , u8 -> u64 , f32, f64
    EQUAL,
    LITERAL,
    RPARA,
    LPARA,
    START
};

std::unordered_map<TokenType, std::string> token_word;

bool is_type(TokenType type);

bool is_expected(const std::vector<Expected>& expected_vector, const TokenType& have);

void REPORT(const std::vector<Expected>& expected_vector, const TokenType& got, std::string file_name);

void LET(const Token& token, AST1_Type& type, std::vector<Expected>& expected_vector);
void IDENT(const Token& token, std::string& ident, std::vector<Expected>& expected_vector);
void COLON(std::vector<Expected>& expected_vector);
void TYPE(const Token& token, std::vector<Expected>& expected_vector, TokenType& dec_type);
void EQUAL(std::vector<Expected>& expected_vector);
void LITERAL(std::vector<Token>& expr, const Token& token);
void PRINT(const Token& token, AST1_Type& type, std::vector<Expected>& expected_vector);
void RPARA(std::vector<Expected>& expected_vector);

void call_token(std::vector<Token>& expr, const Token& token, AST1_Type& type, std::string& ident, std::vector<Expected>& expected_vector, TokenType& dec_type);

std::vector<AST1_NODE> AST1(const std::vector<Token>& line, std::string file_name);