#include "Parser.hpp"
#include <assert.h>
#include <iostream>
#include <variant>
#include <optional>

using namespace Token;

#define log(x) std::clog << "\nLOG:\n" << __FILE__ << ':' << __LINE__ << '\n' << x << "\n\n";

[[nodiscard]] bool Parser::advance() //* fails on fatality
{
    auto result = lexer.next_token();

    if(result)
    {
        previousTok = Tok;
        Tok = peekTok;
        peekTok = *result;
        return true;
    }

    diag_engine.report(std::move(result.error()));

    // indicates failure and fatality
    Tok = {.type=TokenType::EoF};
    peekTok = {.type=TokenType::EoF};

    return false;
}

StmtResult Parser::parse_statement()
{
    using enum TokenType;

    int i = 1;

    if(Tok.is(EoF))
        return StmtError();

    switch(Tok.type)
    {
        case Let:
            return parse_let_dec();
        default:
            log("ERROR: No statement! Advancing!");
            advance();
    }

    return StmtError();
}

AST::Block* Parser::parse_block()
{
    log("Parsing Block.");

    assert(Tok.is(TokenType::LBRAC) 
        && "expected to start with `{`");

    auto loc = Tok.location;

    if(!advance() || Tok.is(TokenType::EoF))
        return {};

    auto* block = arena.make<AST::Block>(loc);

    while(!Tok.is(TokenType::RBRAC))
    {
        if(Tok.is(TokenType::EoF))
        {
            log("Finished Parsing Block.");
            return block;
        }

        if(Tok.is(TokenType::LBRAC))
            block->nodes.emplace_back(parse_block());
    
        else
        {
            auto result = parse_statement();

            if(result.isUsable())
            {
                log("Pushed a Statement.");
                block->nodes.emplace_back(result.get());
            }
        }
    }

    log("Finished Parsing Block.");

    (void)advance();

    return block;
}

AST::Program* Parser::parse()
{
    Tok = lexer.next_token()
        .value_or((Token::Token{.type=TokenType::EoF}));

    peekTok = lexer.next_token()
        .value_or((Token::Token{.type=TokenType::EoF}));

    // todo: parse top level statements

    AST::Program* program;

    log("Parsing Program.");
    
    program = parse_block();

    log("Finished Parsing.")

    AST::Printer printer(std::cout, source_manager);

    printer.print(program);
    
    return program;
}

bool Parser::skip_until(std::vector<Token::TokenType> types)
{
    while(true)
    {
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

ExprResult Parser::parse_expr(SourceManager::SourceLocation begin)
{
    std::cout << "parse_expr()\n";

    auto result = parse_additive(begin);

    if(!result.isUsable())
        return ExprError();

    return result;
}

ExprResult Parser::parse_additive(SourceManager::SourceLocation begin)
{
    std::cout << "parse_additive()\n";
    
    auto result = parse_multiplicative(begin);

    auto begin_loc = Tok.location;

    if(!result.isUsable())
        return ExprError();

    AST::Expr* lhs = result.get();

    while(Tok.is(TokenType::Plus) || Tok.is(TokenType::Subtration))
    {
        auto op = Tok;

        if(!advance())
            return ExprError();

        auto result = parse_multiplicative(begin);

        if(!result.isUsable())
            return ExprError();

        auto* rhs = result.get();

        lhs = arena.make<AST::BinaryExpr>(op, lhs, rhs, SourceManager::SourceLocation{begin_loc.offset, Tok.location.offset - begin_loc.offset, begin_loc.file_id});
    }

    return ExprResult(lhs);
}


ExprResult Parser::parse_multiplicative(SourceManager::SourceLocation begin)
{
    std::cout << "parse_multiplicative()\n";

    auto result = parse_unary(begin);

    if(!result.isUsable())
        return ExprError();

    auto begin_loc = Tok.location;

    AST::Expr* lhs = result.get();

    while(Tok.is(TokenType::Multiplication) || Tok.is(TokenType::Division))
    {
        auto op = Tok;

        if(!advance())
            return ExprError();

        auto result = parse_unary(begin);

        if(!result.isUsable())
            return ExprError();

        auto* rhs = result.get();

        lhs = arena.make<AST::BinaryExpr>(op, lhs, rhs, SourceManager::SourceLocation{begin_loc.offset, Tok.location.offset - begin_loc.offset, begin_loc.file_id});
    }

    return ExprResult(lhs);
}

ExprResult Parser::parse_unary(SourceManager::SourceLocation begin)
{
    std::cout << "parse_unary()\n";

    auto begin_loc = Tok.location;

    if(Tok.is(TokenType::Subtration) || Tok.is(TokenType::Plus))
    {
        auto op = Tok;

        if(!advance())
            return ExprError();

        auto result = parse_unary(begin);

        if(!result.isUsable())
            return ExprError();

        auto* expr = result.get();

        return ExprResult(arena.make<AST::UnaryExpr>(op, expr, SourceManager::SourceLocation{begin_loc.offset, Tok.location.offset - begin_loc.offset, begin_loc.file_id}));
    }

    return parse_primary(begin_loc);
}

ExprResult Parser::parse_primary(SourceManager::SourceLocation begin)
{
    std::cout << "parse_primary()\n";

    if(Tok.is(TokenType::IntegerLiteral))
    {
        auto* expr = arena.make<AST::IntegerLiteralExpr>(Tok, Tok.location);

        if(!advance())
            return ExprError();

        return ExprResult(expr);
    }

    if(Tok.is(TokenType::Identifier))
    {
        auto* expr = arena.make<AST::IdentifierExpr>(Tok, Tok.location);

        if(!advance())
            return ExprError();

        return ExprResult(expr);
    }

    if(Tok.is(TokenType::LPARA))
    {
        if(!advance())
            return ExprError();

        auto result = parse_expr(begin);

        if(!result.isUsable())
            return ExprError();

        auto* expr = result.get();

        if(!Tok.is(TokenType::RPARA))
        { 
            diag(diagExpected(begin, TokenType::RPARA));

            (void)advance();

            return ExprError();
        }

        if(!advance())
            return ExprError();

        return ExprResult(expr);
    }

    diag(diagExpected(begin, "<expr>"));

    return ExprError();
}

TypeResult Parser::parse_type(SourceManager::SourceLocation begin_loc)
{
    auto primitive_types = 
        {   
            TokenType::I8, TokenType::I16, TokenType::I32, TokenType::I64,
            TokenType::U8, TokenType::U16, TokenType::U32, TokenType::U64
        };

    if(!contain(primitive_types, Tok.type))
    {
        diag(diagExpected(begin_loc, "<type>"));
        return TypeError();
    }    

    auto* type = arena.make<AST::Type>(Tok.type, Tok.location);
    
    if(!advance())
        return TypeError();

    return TypeResult(type);
}

StmtResult Parser::parse_let_dec()
{
    using enum TokenType;

    bool isError = false;

    log("Parsing Let Dec.");

    assert(Tok.is(Let) 
            && "expected to start with `let`");

    auto let_begin = Tok.location;

    std::optional<Token::Token> identifier = Token::Token{};

    AST::Type* type = nullptr;
    AST::Expr* expr = nullptr;

    if(!advance())
        return StmtError();

    if(!Tok.is(Identifier))
    {
        isError = true;

        auto valid_continuation_set = {Equal, Colon};

        auto diagnostic = diagExpected(let_begin, Identifier).add_hint(
                Hint(let_begin, Identifier, valid_continuation_set)
            );

        if(!contain(valid_continuation_set, Tok.type))
        {
            diag(diagnostic);

            skip_until({Semicolon, Let});
            if(!Tok.is(Let)) (void)advance(); // to start on the next statement

            return StmtError();
        }
            
        diagnostic.add_higlight(before(Tok, Diag::Highlight::Type::Secondary));

        diag(diagnostic);

        identifier.reset();
    }

    if(identifier.has_value())
    { 
        identifier = Tok;

        if(!advance())
            return StmtError();
    }

    if(Tok.is(Colon))
    {
        if(!advance())
            return StmtError();

        //* parse_type() is responsible for diagnostic
        auto result = parse_type(let_begin);

        if(!result.isUsable())
        {
            skip_until({Semicolon, Let});
            if(!Tok.is(Let)) (void)advance();
            return StmtError();
        }

        type = result.get();
    }

    if(!Tok.is(Equal))
    {
        auto diagnostic = diagExpected(let_begin, Equal);
        
        auto confident_inserts_follow_set = {Identifier, LPARA, IntegerLiteral, FloatLiteral};

        if(contain(confident_inserts_follow_set, Tok.type))
            diagnostic.add_hint
                (
                    Diag::AddHint
                        (
                            let_begin, 
                            Tok.location, 
                            Diag::Message{}
                                .template_id(Diag::DiagnosticID::MaybeInsert)
                                .add_argument(Equal),
                            '='
                        )
                );

        diag(diagnostic);

        skip_until({Semicolon, Let});
        if(!Tok.is(Let)) (void)advance();
        return StmtError();
    }

    if(!advance())
        return StmtError();

    auto result = parse_expr(let_begin);

    if(!result.isUsable())
    {
        skip_until({Semicolon, Let});
        if(!Tok.is(Let)) (void)advance(); // to start on the next statement
        return StmtError();
    }

    expr = result.get();

    if(!Tok.is(Semicolon))
    {
        diag(diagExpected(let_begin, Semicolon)
            .add_hint
            (
                Diag::AddHint
                (
                    let_begin,
                    source_manager.after(previousTok.location), 
                    Diag::Message{}
                        .template_id(Diag::DiagnosticID::MaybeInsert)
                        .add_argument(Semicolon),
                    ';'
                )
            )
        );

        skip_until({Semicolon, Let});
        if(!Tok.is(Let)) (void)advance(); // to start on the next statement
        return StmtError();
    }

    auto* ast = arena.make<AST::LetDec>
    (
        identifier, 
        type, 
        expr, 
        SourceManager::SourceLocation
            {let_begin.offset, Tok.location.offset - let_begin.offset, let_begin.file_id}
    );

    if(!advance()) // to start on the next statement
            return StmtError();

    if(isError)
    {
        auto error_result = StmtError();

        error_result.attach(ast);

        return error_result;
    }

    return StmtResult(ast);
}