#pragma once

#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <expected>
#include "../../Basic/Token.hpp"
#include "../../Support/SourceManager/SourceManager.hpp"
#include "../../Basic/Diagnostic.hpp"
 
namespace fs = std::filesystem;

namespace Lexer
{

    static const std::unordered_map<std::string_view, Token::TokenType> keywords
    {
        {"let", Token::TokenType::Let},
        {"i8", Token::TokenType::I8},
        {"u8", Token::TokenType::U8},
        {"i16", Token::TokenType::I16},
        {"u16", Token::TokenType::U16},
        {"i32", Token::TokenType::I32},
        {"u32", Token::TokenType::U32},
        {"i64", Token::TokenType::I64},
        {"u64", Token::TokenType::U64},
    };

    static const std::unordered_map<Token::TokenType, std::string_view> out_keywords
    {
        {Token::TokenType::Let, "let"},
        {Token::TokenType::Equal, "="},
        {Token::TokenType::Plus, "+"},  
        {Token::TokenType::Identifier, "identifier"},
        {Token::TokenType::IntegerLiteral, "integer literal"},  
    };

    bool is_operator(char c);

    class Lexer 
    {
        private:
            SourceManager::SourceManager& source_manager;
            SourceManager::FileID file_id;
            
            const std::string& contents;

            uint32_t line = 0;
            uint32_t column = 0;
            uint32_t offset = 0;
            
            char current() const;
            char advance();
            char peek() const;

            bool match(const char c);

            bool is_eof() const;
            void skip_space();

            bool is_digit(char c) const;

            std::expected<Token::Token, Diag::Diagnostic> lex_numeric_literal();
            std::expected<Token::Token, Diag::Diagnostic> lex_identifier();
            std::expected<Token::Token, Diag::Diagnostic> lex_operator();

        public:
            Lexer(SourceManager::SourceManager& source_manager, SourceManager::FileID id) : 
                source_manager(source_manager) , file_id(id) , contents(source_manager.get_file(id).contents)
            {}

            std::expected<Token::Token, Diag::Diagnostic> next_token();
    };

}