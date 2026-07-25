#include "../../Lexer/include/Lexer.hpp"
#include "../../AST/AST.hpp"

#include <optional>

namespace Parser
{
    class Parser
    {
        private:
            Lexer::Lexer& lexer;

            SourceManager::SourceManager& source_manager;

            Diag::DiagnosticEngine& diag_engine;

            Token::Token prev_token;
            Token::Token current_token;

            Token::Token previous();
            Token::Token peek();

            bool advance(); //* returns true if it consumed, returns false if it couldn't [[likely]] due to EOF

            bool expect(Token::TokenType expected_type); //* returns true if type == expected type and advances, else returns false
            bool expect_type();

            std::expected<AST::AST, Diag::Diagnostic> parse_type();

            std::expected<AST::AST, Diag::Diagnostic> parse_expr();

            std::expected<AST::AST, std::vector<Diag::Diagnostic>> parse_let_dec();

        public:
            Parser(Lexer::Lexer& lexer, SourceManager::SourceManager& source_manager, Diag::DiagnosticEngine& diag_engine)
                : lexer(lexer), source_manager(source_manager), diag_engine(diag_engine)
            {
                auto result = lexer.next_token();

                if(result) current_token = *result;
                else diag_engine.report(std::move(result.error()));
            }

            std::optional<AST::AST> parse();
    };
}