#include "Parser.hpp"

void Parser::diag(Diag::DiagnosticBuilder diagnostic)
{
    diag_engine.report(diagnostic.build());
}

Diag::DiagnosticBuilder Parser::diagExpected(Token::TokenType expected)
{
    Diag::DiagnosticBuilder diag_builder;

    diag_builder
        .id(Diag::DiagnosticID::Auto)
        .severity(Diag::Severity::Error)
        .primary_location(Tok.location)
        .add_higlight
        (
            Diag::Highlight{}
                .primary()
                .location(Tok.location)
                .message
                (
                    Diag::Message{}
                        .template_id(Diag::DiagnosticID::Auto)
                        .add_argument(Token::token_type_name(expected))
                        .add_argument(Token::pretty_token(Tok.type, source_manager.get_string(Tok.location)))
                )
        );

    return diag_builder;
}

Diag::DiagnosticBuilder Parser::diagExpected(std::string expected)
{
    Diag::DiagnosticBuilder diag_builder;

    diag_builder
        .id(Diag::DiagnosticID::Auto)
        .severity(Diag::Severity::Error)
        .primary_location(Tok.location)
        .add_higlight
        (
            Diag::Highlight{}
                .primary()
                .location(Tok.location)
                .message
                (
                    Diag::Message{}
                        .template_id(Diag::DiagnosticID::Auto)
                        .add_argument(expected)
                        .add_argument(Token::pretty_token(Tok.type, source_manager.get_string(Tok.location)))
                )
        );

    return diag_builder;
}

Diag::Highlight Parser::before(Token::Token Tok, Diag::Highlight::Type p_type = Diag::Highlight::Type::Primary)
{
    return 
        Diag::Highlight{.type=p_type}
            .location(Tok.location)
            .message
            (
                Diag::Message{}
                    .template_id(Diag::DiagnosticID::ExpectedBefore)
                    .add_argument(Token::pretty_token(Tok.type, source_manager.get_string(Tok.location)))
            );
}

Diag::FixItHint Parser::Hint(Token::TokenType expected, std::vector<Token::TokenType> types)
{
    bool in = false;
    for(Token::TokenType type : types) if(Tok.is(type)) in = true;

    if(in)
    {
        return 
            Diag::AddHint
            {
                .message=Diag::Message{}
                    .template_id(Diag::DiagnosticID::MaybeInsert)
                    .add_argument(Token::token_type_string(expected)),
                .location=SourceManager::before(Tok.location),
                .add=Token::token_type_string(expected)
            };
    }

    return 
        Diag::ReplaceHint
        {
            .message=Diag::Message{}
                .template_id(Diag::DiagnosticID::MaybeReplace)
                .add_argument(Token::pretty_token(Tok.type, source_manager.get_string(Tok.location)))
                .add_argument(Token::token_type_string(expected)),
            .location=Tok.location,
            .replace=Token::token_type_string(expected)
        };
}

Diag::FixItHint Parser::Hint(std::string expected, std::vector<Token::TokenType> types)
{
    bool in = false;
    for(Token::TokenType type : types) if(Tok.is(type)) in = true;

    if(in)
    {
        return 
            Diag::AddHint
            {
                .message=Diag::Message{}
                    .template_id(Diag::DiagnosticID::MaybeInsert)
                    .add_argument(expected),
                .location=Tok.location,
                .add=expected
            };
    }

    return 
        Diag::ReplaceHint
        {
            .message=Diag::Message{}
                .template_id(Diag::DiagnosticID::MaybeReplace)
                .add_argument(Token::pretty_token(Tok.type, source_manager.get_string(Tok.location)))
                .add_argument(expected),
            .location=Tok.location,
            .replace=expected
        };
}