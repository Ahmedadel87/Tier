#pragma once

#include <memory>
#include <vector>
#include <ostream>
#include "Token.hpp"
#include "SourceManager.hpp"

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

    // note: blocks are statements

    struct Block;

    using Program = Block;

    using Stmt = std::variant<LetDec, Block>;

    struct Block : public Node
    {
        std::vector<Stmt> nodes;
    };

    class Printer
    {
        private:
            std::ostream& m_out;
            SourceManager::SourceManager& m_source_manager;

            void print_n_spaces(size_t amount)
            {
                for(int i = 0; i < amount; ++i) m_out << ' ';
            }

            void print_expr(AST::Expr* expr, uint16_t indent = 0)
            {
                if(expr->is(AST::Expr::Kind::BinaryExpr))
                {
                    auto* binary_expr = static_cast<AST::BinaryExpr*>(expr);
                    
                    for(int i = 0; i < indent; ++i) m_out << "  ";

                    m_out << token_type_string(binary_expr->op.type) << '\n';

                    print_expr(binary_expr->lhs, indent+1);
                    print_expr(binary_expr->rhs, indent+1);

                    return;
                }

                if(expr->is(AST::Expr::Kind::UnaryExpr))
                {
                    auto* unary_expr = static_cast<AST::UnaryExpr*>(expr);
                    
                    for(int i = 0; i < indent; ++i) m_out << "  ";

                    m_out << token_type_string(unary_expr->op.type) << '\n';

                    print_expr(unary_expr->expr, indent+1);

                    return;
                }

                if(expr->is(AST::Expr::Kind::IntegerLiteralExpr))
                {
                    auto* integer_literal_expr = static_cast<AST::IntegerLiteralExpr*>(expr);
                    
                    for(int i = 0; i < indent; ++i) m_out << "  ";

                    m_out << m_source_manager.get_string(integer_literal_expr->integer_literal.location);

                    m_out << '\n';

                    return;
                }

                if(expr->is(AST::Expr::Kind::IdentifierExpr))
                {
                    auto* identifier_expr = static_cast<AST::IdentifierExpr*>(expr);
                    
                    for(int i = 0; i < indent; ++i) m_out << "  ";

                    m_out << m_source_manager.get_string(identifier_expr->identifier.location);

                    m_out << '\n';

                    return;
                }
            }

            void print(const LetDec& let, int indent = 0)
            {
                print_n_spaces(indent);
                m_out << "LetDec:" << '\n';
                print_n_spaces(indent+4);
                m_out << "Identifier: " << let.identifier << '\n';
                print_n_spaces(indent+4);
                m_out << "Type: " << Token::token_type_name(let.type.type) << '\n';
                print_n_spaces(indent+4);
                m_out << "Expr: " << '\n';
                print_expr(let.expr, indent);
                m_out << '\n';
            }

            void print_block(const Block& block, int indent = 0)
            {
                print_n_spaces(indent);
                m_out << "Block {\n";

                for(const Stmt& stmt : block.nodes)
                {
                    if (auto* inner_block = std::get_if<Block>(&stmt))
                    {
                        print_block(*inner_block, indent+4);
                    }

                    else
                    {
                        std::visit(
                            [this, indent](const auto& node)
                            {
                                print(node, indent + 4);
                            },
                            stmt
                        );
                    }
                }

                print_n_spaces(indent);
                m_out << "}\n";
            }

        public:
            Printer(std::ostream& out, SourceManager::SourceManager& source_manager)
                : m_out(out), m_source_manager(source_manager)
            {}

            void print(const Program& program, int indent = 0)
            {
                m_out << "Program: \n";
                print_block(program, indent+4);
            }
    };
}