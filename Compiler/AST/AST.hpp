#include <memory>
#include <vector>
#include "Token.hpp"

namespace AST
{

    struct Expr
    {
        enum class Kind
        {
            BinaryExpr
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

}