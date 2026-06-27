#pragma once

#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <expected>
#include "../../Basic/Token.h"
#include "../../Support/SourceManager/SourceManager.hpp"
#include "../../Basic/Diagnostic.h"

namespace fs = std::filesystem;

namespace Lexer
{

    static const std::unordered_map<std::string_view, Token::TokenType> keywords
    {
        {"let", Token::TokenType::Let},
    };

    static const std::unordered_map<Token::TokenType, std::string_view> out_keywords
    {
        {Token::TokenType::Let, "let"},
        {Token::TokenType::Equal, "="},
        {Token::TokenType::Plus, "+"},  
        {Token::TokenType::Identifier, "identifier"},
        {Token::TokenType::IntegerLiteral, "integer literal"},
        {Token::TokenType::Semicolon, "semi-colon"}  
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

            std::expected<Token::Token, Diagnostic::LexError> lex_numeric_literal();
            std::expected<Token::Token, Diagnostic::LexError> lex_identifier();
            std::expected<Token::Token, Diagnostic::LexError> lex_operator();

        public:
            Lexer(SourceManager::SourceManager& source_manager, SourceManager::FileID id) : 
                source_manager(source_manager) , file_id(id) , contents(source_manager.get_file(id).contents)
            {}

            std::expected<Token::Token, Diagnostic::LexError> next_token();
    };

}