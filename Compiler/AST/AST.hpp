#include <memory>
#include <vector>
#include "Token.hpp"

namespace AST
{
    struct Node
    {
        SourceManager::SourceLocation source_loc;
    };

    struct Expr : public Node
    {
        enum class Kind
        {
            BinaryExpr,
            UnaryExpr,
            IntegerLiteralExpr,
            IdentifierExpr
        };

        Kind kind;

        explicit Expr(Kind kind)
            : kind(kind)
        {}

        bool is(Kind p_kind)
        {
            return p_kind == kind;
        }

        Expr() = default;

        virtual ~Expr() = default;
    };

    struct BinaryExpr : public Expr
    {
        Token::Token op;

        Expr* lhs;
        Expr* rhs;
    
        explicit BinaryExpr(Token::Token& op, Expr* lhs, Expr* rhs)
            : Expr(Kind::BinaryExpr) , op(op), lhs(lhs), rhs(rhs)
        {}
    };

    struct UnaryExpr : public Expr
    {
        Token::Token op;
        Expr* expr;
    
        explicit UnaryExpr(Token::Token& op, Expr* expr)
            : Expr(Kind::UnaryExpr) , op(op), expr(expr)
        {}
    };

    struct IntegerLiteralExpr : public Expr
    {
        Token::Token integer_literal;
    
        explicit IntegerLiteralExpr(Token::Token& integer_literal)
            : Expr(Kind::IntegerLiteralExpr) , integer_literal(integer_literal)
        {}
    };

    struct IdentifierExpr : public Expr
    {
        Token::Token identifier;
    
        explicit IdentifierExpr(Token::Token& identifier)
            : Expr(Kind::IdentifierExpr) , identifier(identifier)
        {}
    };

    struct Type : public Node
    {
        Token::TokenType type;

        Type(Token::TokenType type) : type(type)
        {}

        Type() = default;
    };

    struct LetDec : public Node
    {
        std::string identifier;
        Type type;
        Expr* expr;

        LetDec(std::string identifier, Type type, Expr* expr)
            : identifier(identifier), type(type), expr(expr)
        {}

        LetDec() = default;
    };

    using Stmt = std::variant<LetDec>;
}