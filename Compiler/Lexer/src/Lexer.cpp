#include "../include/Lexer.hpp"
#include "../../Support/SourceManager/SourceManager.hpp"
#include <variant>
#include <cctype>
#include <expected>

namespace Lexer
{    
    char Lexer::current() const
    {
        if(offset >= contents.size()) return '\0';
        return contents[offset];
    }

    char Lexer::peek() const
    {
        if(offset + 1 >= contents.size()) return '\0';

        return contents[offset+1];
    }

    char Lexer::advance()
    {
        ++offset;
        if(offset >= contents.size()) return '\0';
        return contents[offset];
    }

    bool Lexer::match(const char c)
    {
        if(current() == c)
        {
            advance();
            return true;
        }
        
        return false;
    }

    bool Lexer::is_eof() const
    {
        return current() == '\0';
    }

    bool is_operator(char c)
    {
        switch(c)
        {
            case '=':
                return true;
            case '-':
                return true;
            case '+':
                return true;
            case '/':
                return true;
            case '*':
                return true;
            case ';':
                return true;
            case '!':
                return true;
            case 'j':
                return true;
            default:
                return false;
        }
    }

    Token::TokenType tokenize_operator(char c)
    {
        switch(c)
        {
            case '=':
                return Token::TokenType::Equal;
            case '-':
                return Token::TokenType::Subtration;
            case '+':
                return Token::TokenType::Plus;
            case '/':
                return Token::TokenType::Division;
            case '*':
                return Token::TokenType::Multiplication;
            case ';':
                return Token::TokenType::Semicolon;
            case '!':
                return Token::TokenType::Identifier;
            default:
                return Token::TokenType::Identifier;
        }
    }

    bool Lexer::is_digit(char c) const
    {
        if(c >= '0' && c <= '9') return true;
        return false;
    }

    Token::TokenType tokenize_keyword(const std::string_view str)
    {
        auto it = keywords.find(str);
        if(it != keywords.end()) return it->second;
        else return Token::TokenType::Identifier;
    }

	bool is_valid_ident_char(char c){
		return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
	}

    void Lexer::skip_space()
    {
        while(
            offset < contents.size() && 
			std::isspace(static_cast<unsigned char>(current()))
        )
        {
            advance();
        }
    }

    std::expected<Token::Token, Diagnostic::LexError> Lexer::lex_numeric_literal()
    {
        uint32_t token_offset = offset;
        uint16_t length = 0;

        while(is_digit(current()))
        {
            length++;
            advance();
        }

        return Token::Token{Token::TokenType::IntegerLiteral, SourceManager::SourceLocation{token_offset, length, source_manager.get_file(file_id).id}};
    }

    std::expected<Token::Token, Diagnostic::LexError> Lexer::lex_identifier()
    {
        uint32_t token_offset = offset;
		uint16_t length = 0;

        while(!is_eof() && is_valid_ident_char(current()))
        {
            length++;
            advance();
        }

        return Token::Token{tokenize_keyword(source_manager.get_string({token_offset, length})), SourceManager::SourceLocation{token_offset, length, source_manager.get_file(file_id).id}};
    }

    std::expected<Token::Token, Diagnostic::LexError> Lexer::lex_operator()
    {
        Token::Token op_token = {tokenize_operator(current()), SourceManager::SourceLocation{offset, 1, file_id}};
        advance();
        return op_token;  
    }

    std::expected<Token::Token, Diagnostic::LexError> Lexer::next_token()
    {
        //* Lexer expects to start after the last token.

        skip_space();

        if(is_eof()) 
            return Token::Token{Token::TokenType::EoF};

        if(is_digit(current()))
            return lex_numeric_literal();
        
        if(is_operator(current()))
            return lex_operator();

        if(is_valid_ident_char(current()))
            return lex_identifier();

        return std::unexpected(Diagnostic::LexError{"Unexpected Character"});
    }
}