#include <memory>
#include <vector>
#include "Token.hpp"

namespace AST
{

    struct Expr
    {
        enum class Kind
        {
            BinaryExpr,
            UnaryExpr
        };

        Kind kind;

        explicit Expr(Kind kind)
            : kind(kind)
        {}

        virtual ~Expr() = default;
    };

    struct BinaryExpr : public Expr
    {
        Token::Token op;

        Expr* lhs;
        Expr* rhs;
    
        explicit BinaryExpr(Token::Token&& op, Expr* lhs, Expr* rhs)
            : Expr(Kind::BinaryExpr) , op(op), lhs(lhs), rhs(rhs)
        {}
    };

    struct UnaryExpr : public Expr
    {
        Token::Token op;
        Expr* expr;
    
        explicit UnaryExpr(Token::Token&& op, Expr* expr)
            : Expr(Kind::UnaryExpr) , op(op), expr(expr)
        {}
    };

    struct AST
    {};

}