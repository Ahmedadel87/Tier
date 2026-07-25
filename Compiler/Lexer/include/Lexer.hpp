#pragma once

#include <expected>

#include "../../Basic/Token.hpp"
#include "../../Support/SourceManager/SourceManager.hpp"
#include "../../Basic/Diagnostic.hpp"
 
namespace Lexer
{
    bool is_operator(char c);

    class Lexer 
    {
        private:
            SourceManager::SourceManager& source_manager;
            SourceManager::FileID file_id;

            const std::string& contents;

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

            //* note: it is expected that the caller of the next_token to report the Diagnostic to the DiagnosticEngine
            std::expected<Token::Token, Diag::Diagnostic> next_token();
    };

}