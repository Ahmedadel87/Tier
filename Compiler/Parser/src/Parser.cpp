#include "Parser.hpp"
#include <assert.h>
#include <iostream>

using namespace Token;

[[nodiscard]] bool Parser::advance() //* fails on fatality
{
    auto result = lexer.next_token();

    if(result)
    {
        Tok = peekTok;
        peekTok = *result;
        return true;
    }

    diag_engine.report(std::move(result.error()));

    return false;
}

void Parser::init()
{
    while(true)
    {
        auto result = lexer.next_token();

        if(result)
        { 
            Tok = *result;

            auto result2 = lexer.next_token();

            if(result2) peekTok = *result2; 

            return;
        }
    }
}

bool Parser::skip_until(std::vector<Token::TokenType> types)
{
    while(true)
    {
        std::cout << "\nSkipLog: " << Token::token_type_name(Tok.type) << '\n';

        if(Tok.is(Token::TokenType::EoF))
            return false;

        for(Token::TokenType type : types)
            if(Tok.is(type))
                return true;

        if(!advance())
            return false;
    }

    return true;
}

StmtResult Parser::parse()
{
    init(); //* starts us on a valid token.

    using enum TokenType;

    int i = 1;

    while(true)
    {
        std::cout << "\nline: " << i++ << '\n';

        if(Tok.is(EoF))
            break;

        switch(Tok.type)
        {
            case Let:
                parse_let_dec();
        }
    }

    return StmtError();
}

void Parser::print_expr(AST::Expr* expr, uint16_t depth = 0)
{
    if(expr->is(AST::Expr::Kind::BinaryExpr))
    {
        auto* binary_expr = static_cast<AST::BinaryExpr*>(expr);
        
        for(int i = 0; i < depth; ++i) std::cout << "  ";

        std::cout << token_type_string(binary_expr->op.type) << '\n';

        print_expr(binary_expr->lhs, depth+1);
        print_expr(binary_expr->rhs, depth+1);

        return;
    }

    if(expr->is(AST::Expr::Kind::UnaryExpr))
    {
        auto* unary_expr = static_cast<AST::UnaryExpr*>(expr);
        
        for(int i = 0; i < depth; ++i) std::cout << "  ";

        std::cout << token_type_string(unary_expr->op.type) << '\n';

        print_expr(unary_expr->expr, depth+1);

        return;
    }

    if(expr->is(AST::Expr::Kind::IntegerLiteralExpr))
    {
        auto* integer_literal_expr = static_cast<AST::IntegerLiteralExpr*>(expr);
        
        for(int i = 0; i < depth; ++i) std::cout << "  ";

        std::cout << source_manager.get_string(integer_literal_expr->integer_literal.location);

        std::cout << '\n';

        return;
    }

    if(expr->is(AST::Expr::Kind::IdentifierExpr))
    {
        auto* identifier_expr = static_cast<AST::IdentifierExpr*>(expr);
        
        for(int i = 0; i < depth; ++i) std::cout << "  ";

        std::cout << source_manager.get_string(identifier_expr->identifier.location);

        std::cout << '\n';

        return;
    }

}

bool Parser::is_expr_terminator(TokenType type)
{
    return
        type == TokenType::Semicolon
        || type == TokenType::RPARA;
}

ExprResult Parser::parse_expr()
{
    std::cout << "parse_expr()\n";

    auto result = parse_additive();

    if(!result.isUsable())
        return ExprError();

    if(!is_expr_terminator(Tok.type))
    {
        diag
        (
            diagExpected("<op>")
            .add_hint
            (
                Hint
                (
                    "<op>", 
                    {TokenType::IntegerLiteral, TokenType::Identifier}
                )
            )
        );

        return ExprError();
    }

    return result;
}

ExprResult Parser::parse_additive()
{
    std::cout << "parse_additive()\n";
    
    auto result = parse_multiplicative();

    if(!result.isUsable())
        return ExprError();

    AST::Expr* lhs = result.get();

    while(Tok.is(TokenType::Plus) || Tok.is(TokenType::Subtration))
    {
        auto op = Tok;

        if(!advance())
            return ExprError();

        auto result = parse_multiplicative();

        if(!result.isUsable())
            return ExprError();

        auto* rhs = result.get();

        lhs = arena.make<AST::BinaryExpr>(op, lhs, rhs);
    }

    return ExprResult(lhs);
}


ExprResult Parser::parse_multiplicative()
{
    std::cout << "parse_multiplicative()\n";

    auto result = parse_unary();

    if(!result.isUsable())
        return ExprError();

    AST::Expr* lhs = result.get();

    while(Tok.is(TokenType::Multiplication) || Tok.is(TokenType::Division))
    {
        auto op = Tok;

        if(!advance())
            return ExprError();

        auto result = parse_unary();

        if(!result.isUsable())
            return ExprError();

        auto* rhs = result.get();

        lhs = arena.make<AST::BinaryExpr>(op, lhs, rhs);
    }

    return ExprResult(lhs);
}

ExprResult Parser::parse_unary()
{
    std::cout << "parse_unary()\n";

    if(Tok.is(TokenType::Subtration) || Tok.is(TokenType::Plus))
    {
        auto op = Tok;

        if(!advance())
            return ExprError();

        auto result = parse_unary();

        if(!result.isUsable())
            return ExprError();

        auto* expr = result.get();

        return ExprResult(arena.make<AST::UnaryExpr>(op, expr));
    }

    return parse_primary();
}

ExprResult Parser::parse_primary()
{
    std::cout << "parse_primary()\n";
    
    if(Tok.is(TokenType::IntegerLiteral))
    {
        auto* expr = arena.make<AST::IntegerLiteralExpr>(Tok);

        if(!advance())
            return ExprError();

        return ExprResult(expr);
    }

    if(Tok.is(TokenType::Identifier))
    {
        auto* expr = arena.make<AST::IdentifierExpr>(Tok);

        if(!advance())
            return ExprError();

        return ExprResult(expr);
    }

    if(Tok.is(TokenType::LPARA))
    {
        if(!advance())
            return ExprError();

        auto result = parse_expr();

        if(!result.isUsable())
            return ExprError();

        auto* expr = result.get();

        if(!Tok.is(TokenType::RPARA))
        { 
            diag(diagExpected(TokenType::RPARA));

            return ExprError();
        }

        if(!advance())
            return ExprError();

        return ExprResult(expr);
    }

    diag(diagExpected("<expr>"));

    return ExprError();
}

TypeResult Parser::parse_type()
{
    if(!Tok.is(Token::TokenType::I32))
    {
        diagExpected("<type>");
        return TypeError();
    }    

    AST::Type type(Tok.type);
    
    if(!advance())
        return TypeError();

    return TypeResult(type);
}

StmtResult Parser::parse_let_dec()
{
    using enum TokenType;

    assert(Tok.is(Let) 
            && "expected to start with `let`");

    auto let_loc = Tok.location;

    std::string identifier;
    AST::Type type;
    AST::Expr* expr;

    if(!advance())
        return StmtError();

    if(!Tok.is(Identifier))
    {
        diag
        (
            diagExpected(Identifier)
            .add_hint
            (
                Hint(Identifier, {Equal, Colon})
            )
        );

        skip_until({Semicolon, LBRAC});
        (void)advance(); // to start on the next statement
        return StmtError();
    }

    identifier = source_manager.get_string(Tok.location);

    if(!advance())
        return StmtError();

    if(Tok.is(Colon))
    {
        if(!advance())
            return StmtError();

        //* parse_type() is responsible for diagnostic
        auto result = parse_type();

        if(!result.isUsable())
        {
            skip_until({Semicolon, LBRAC});
            return StmtError();
        }

        type = result.get();
    }

    if(!Tok.is(Equal))
    {
        diag(diagExpected(Equal));

        skip_until({Semicolon, LBRAC});
        (void)advance(); // to start on the next statement
        return StmtError();
    }

    if(!advance())
        return StmtError();

    auto result = parse_expr();

    if(!result.isUsable())
    {
        std::clog << "PARSING FAILED.\n";

        skip_until({Semicolon, LBRAC});
        (void)advance(); // to start on the next statement
        return StmtError();
    }

    expr = result.get();

    print_expr(expr);

    if(!Tok.is(Semicolon))
    {
        skip_until({Semicolon, LBRAC});
        (void)advance(); // to start on the next statement
        return StmtError();
    }

    if(!advance()) // to start on the next statement
            return StmtError();

    return StmtResult(AST::LetDec{identifier, type, expr});
}