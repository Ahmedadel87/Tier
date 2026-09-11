#pragma once

#include "Token.hpp"
#include "Lexer.hpp"
#include "SourceManager.hpp"
#include "Diagnostics.hpp"
#include "AST.hpp"
#include "MemoryManager.hpp"

template<typename AST>
class ActionResult
{
    private:
        bool usable;

        AST ast;

    public:
        explicit ActionResult(bool p_usable)
        {
            usable = p_usable;
        }

        explicit ActionResult(AST p_ast)
        {
            usable = true;
            ast = std::move(p_ast);
        }

        bool isUsable() const
        {
            return usable;
        }

        AST& get()
        {
            return ast;
        }
};

using StmtResult = ActionResult<AST::Stmt>;
using ExprResult = ActionResult<AST::Expr*>;
using TypeResult = ActionResult<AST::Type>;

inline StmtResult StmtError() { return StmtResult(false); }
inline ExprResult ExprError() { return ExprResult(false); }
inline TypeResult TypeError() { return TypeResult(false); }

class Parser
{
    private:

        Mem::Arena<1024> arena; //* each block is 1024 bytes (1 kilobyte)

        Lexer::Lexer& lexer;
        SourceManager::SourceManager& source_manager;

        // Diagnostics

        Diag::DiagnosticEngine& diag_engine;

        void diag(Diag::DiagnosticBuilder diagnostic);

        Diag::Highlight before(Token::Token Tok, Diag::Highlight::Type highlight_type = Diag::Highlight::Type::Primary);

        Diag::DiagnosticBuilder diagExpected(SourceManager::SourceLocation begin, Diag::DiagnosticArgument expected);
        Diag::FixItHint Hint(SourceManager::SourceLocation begin, Diag::DiagnosticArgument expected, std::vector<Token::TokenType> types);    
        
        // Recovery
        // returns false on advance fatality
        bool skip_until(std::vector<Token::TokenType> types);

        Token::Token Tok; //* current tok
        Token::Token peekTok; //* next tok

        bool advance(); //* fails on fatality

        bool is_expr_terminator(Token::TokenType type);

        ExprResult parse_expr(SourceManager::SourceLocation begin);
        ExprResult parse_additive(SourceManager::SourceLocation begin);
        ExprResult parse_multiplicative(SourceManager::SourceLocation begin);
        ExprResult parse_unary(SourceManager::SourceLocation begin);
        ExprResult parse_primary(SourceManager::SourceLocation begin);

        void print_expr(AST::Expr* expr, uint16_t depth);

        TypeResult parse_type(SourceManager::SourceLocation begin);
        StmtResult parse_let_dec();

        StmtResult parse_statement();
        AST::Block parse_block();

    public:
        Parser(Lexer::Lexer& lexer, SourceManager::SourceManager& source_manager, Diag::DiagnosticEngine& diag_engine)
            : lexer(lexer), source_manager(source_manager), diag_engine(diag_engine)
        {}

        AST::Program parse();
};