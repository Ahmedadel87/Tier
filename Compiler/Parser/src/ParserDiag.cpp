#include "Parser.hpp"

void Parser::diag(Diag::DiagnosticBuilder diagnostic)
{
    diag_engine.report(diagnostic.build());
}

Diag::DiagnosticBuilder Parser::diagExpected(SourceManager::SourceLocation begin, Diag::DiagnosticArgument expected)
{
    Diag::DiagnosticBuilder diag_builder;

    diag_builder
        .id(Diag::DiagnosticID::Auto)
        .severity(Diag::Severity::Error)
        .primary_location(Tok.location)
        .begin_location(begin)
        .message
        (
            Diag::Message{}
                .template_id(Diag::DiagnosticID::Auto)
                .add_argument(expected)
                .add_argument(Tok)
        )
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
                        .add_argument(Tok)
                )
        );

    return diag_builder;
}

Diag::Highlight Parser::before(Token::Token Tok, Diag::Highlight::Type highlight_type)
{
    return 
        Diag::Highlight{.type=highlight_type}
            .location(source_manager.before(Tok.location))
            .message
            (
                Diag::Message{}
                    .template_id(Diag::DiagnosticID::ExpectedBefore)
                    .add_argument(Tok)
            );
}

Diag::FixItHint Parser::Hint(SourceManager::SourceLocation begin, Diag::DiagnosticArgument expected, std::vector<Token::TokenType> types)
{
    bool in = false;
    for(Token::TokenType type : types) if(Tok.is(type)) in = true;

    if(in)
    {
        return 
            Diag::AddHint
            (
                begin,
                source_manager.before(Tok.location),
                Diag::Message{}
                    .template_id(Diag::DiagnosticID::MaybeInsert)
                    .add_argument(expected),
                expected
            );
    }

    return 
        Diag::ReplaceHint
        (
            begin,
            Tok.location,
            Diag::Message{}
                .template_id(Diag::DiagnosticID::MaybeReplace)
                .add_argument(Tok)
                .add_argument(expected),
            expected
        );
}