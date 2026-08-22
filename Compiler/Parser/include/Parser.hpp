#include "Token.hpp"
#include "Lexer.hpp"
#include "SourceManager.hpp"
#include "Diagnostic.hpp"
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

        Diag::Highlight before(Token::Token Tok, Diag::Highlight::Type p_type);

        Diag::DiagnosticBuilder diagExpected(Token::TokenType expected);
        Diag::DiagnosticBuilder diagExpected(std::string expected);
    
        Diag::FixItHint Hint(Token::TokenType expected, std::vector<Token::TokenType> types);
        Diag::FixItHint Hint(std::string expected, std::vector<Token::TokenType> types);
    
        // Recovery
        // returns false on advance fatality
        bool skip_until(std::vector<Token::TokenType> types);

        Token::Token Tok; //* current tok
        Token::Token peekTok; //* next tok

        bool advance(); //* fails on fatality

        bool is_expr_terminator(Token::TokenType type);

        ExprResult parse_expr();
        ExprResult parse_additive();
        ExprResult parse_multiplicative();
        ExprResult parse_unary();
        ExprResult parse_primary();

        void print_expr(AST::Expr* expr, uint16_t depth);

        TypeResult parse_type();
        StmtResult parse_let_dec();

        void init();

    public:
        Parser(Lexer::Lexer& lexer, SourceManager::SourceManager& source_manager, Diag::DiagnosticEngine& diag_engine)
            : lexer(lexer), source_manager(source_manager), diag_engine(diag_engine)
        {}

        StmtResult parse();
};