#include "Parser.hpp"

void Parser::diagExpected(Token::TokenType expected)
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

    diag_engine.report(diag_builder.build());
}

void Parser::diagExpected(std::string expected)
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

    diag_engine.report(diag_builder.build());
}

void Parser::diagExpected(Token::TokenType expected, Diag::FixItHint hint)
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
        )
        .add_hint(hint);

    diag_engine.report(diag_builder.build());
}

void Parser::diagExpected(std::string expected, Diag::FixItHint hint)
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
        )
        .add_hint(hint);

    diag_engine.report(diag_builder.build());
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
                .location=before(Tok.location),
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