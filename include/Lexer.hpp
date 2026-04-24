#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>

#include <Options.hpp>

enum class TokenType{

    // Literals
    INT,
    FLOAT,
    STR,

    IDENTIFIER,

    // Types
    I8,
    U8,
    I16,
    U16,
    I32,
    U32,
    I64,
    U64,
    F32,
    F64,
    MUT,

    // Punctuation
    LBRAC,    // '{'
    RBRAC,   // '}'
    LPARA,  // '('
    RPARA, // ')'
    COLON, // ':'
    HASHTAG, // '#'

    // Operators
    EQUAL,
    ADD,
    SUB,
    MUL,
    DIV,

    // Keywords
    LET,
    PRINT,
    SET,
    ENFORCE,
    INFER,
    ON,
    INSTEAD,
    TYPEGROUP,

    NULL_
};

struct lexical_token{
    size_t line;
    std::string string;
    size_t org_start_pos;
    bool is_string;
    std::string org_line;
};

struct Token{
    size_t org_start_pos;
    size_t line_num;
    std::string org_word;
    std::string org_line;
    TokenType type;
};

struct is_num_is_decimal{
    bool valid;
    bool decimal;
};

is_num_is_decimal isdigitstr(std::string str);
bool only_space(std::string str);
std::string trim(std::string& s);
std::vector<std::vector<lexical_token>> Stream(std::vector<std::string> lines);
std::vector<Token> Tokenize(std::vector<lexical_token> line);

static const std::unordered_map<std::string, TokenType> keywords{
    {"let", TokenType::LET},
    {":", TokenType::COLON},
    {"i8", TokenType::I8},
    {"u8", TokenType::U8},
    {"i16", TokenType::I16},
    {"u16", TokenType::U16},
    {"i32", TokenType::I32},
    {"u32", TokenType::U32},
    {"i64", TokenType::I64},
    {"u64", TokenType::U64},
    {"f32", TokenType::F32},
    {"f64", TokenType::F64},
    {"{", TokenType::LBRAC},
    {"}", TokenType::RBRAC},
    {"(", TokenType::LPARA},
    {")", TokenType::RPARA},
    {"=", TokenType::EQUAL},
    {"+", TokenType::ADD},
    {"-", TokenType::SUB},
    {"*", TokenType::MUL},
    {"/", TokenType::DIV},
    {"print", TokenType::PRINT},
    {"#", TokenType::HASHTAG},
    {"set", TokenType::SET},
    {"enforce", TokenType::ENFORCE},
    {"infer", TokenType::INFER},
    {"on", TokenType::ON},
    {"instead", TokenType::INSTEAD},
    {"typegroup", TokenType::TYPEGROUP},
    {"mut", TokenType::MUT},
};

static const std::unordered_map<TokenType, std::string> token_word = {
    {TokenType::LET, "LET"},
    {TokenType::COLON, "COLON"},
    {TokenType::I8, "I8"},
    {TokenType::U8, "U8"},
    {TokenType::I16, "I16"},
    {TokenType::U16, "U16"},
    {TokenType::I32, "I32"},
    {TokenType::U32, "U32"},
    {TokenType::I64, "I64"},
    {TokenType::U64, "U64"},
    {TokenType::F32, "F32"},
    {TokenType::F64, "F64"},
    {TokenType::LBRAC, "LBRAC"},
    {TokenType::RBRAC, "RBRAC"},
    {TokenType::LPARA, "LPARA"},
    {TokenType::RPARA, "RPARA"},
    {TokenType::EQUAL, "EQUAL"},
    {TokenType::ADD, "+"},
    {TokenType::SUB, "-"},
    {TokenType::MUL, "*"},
    {TokenType::DIV, "/"},
    {TokenType::PRINT, "PRINT"},
    {TokenType::INT, "INT"},
    {TokenType::FLOAT, "FLOAT"},
    {TokenType::STR, "STR"},
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::HASHTAG, "HASHTAG"},
    {TokenType::SET, "set"},
    {TokenType::ENFORCE, "enforce"},
    {TokenType::INFER, "infer"},
    {TokenType::ON, "on"},
    {TokenType::INSTEAD, "instead"},
    {TokenType::TYPEGROUP, "typegroup"},
    {TokenType::MUT, "mut"},
};