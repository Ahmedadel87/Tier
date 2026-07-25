#include "../../Lexer/include/Lexer.hpp"
#include "../../Support/SourceManager/SourceManager.hpp"
#include "../../Basic/Token.hpp"
#include "../../Basic/Diagnostic.hpp"
#include "../include/Parser.hpp"

#include <iostream>

namespace Parser
{
    Token::Token Parser::peek()
    {
        return current_token;
    }

    Token::Token Parser::previous()
    {
        return prev_token;
    }

    bool Parser::advance() //* returns true if it consumed, returns false if it couldn't [[likely]] due to EOF
    {
        prev_token = current_token;

        auto result = lexer.next_token();

        if(result) 
        {
            if(result->type == Token::TokenType::EoF) return false;

            current_token = *result;

            return true;
        }

        else
        {
            diag_engine.report(std::move(result.error()));
            
            return false;
        }
    }

    bool Parser::expect(Token::TokenType expected_type) //* returns true if type == expected type and advances, else returns false
    {
        if(peek().type == expected_type)
        {
            if(advance()) return true;
        }

        return false;
    }

    bool Parser::expect_type()
    {
        switch(peek().type)
        {
            case Token::TokenType::I8:  if(advance()) return true;
            case Token::TokenType::U8:  if(advance()) return true;
            case Token::TokenType::I16: if(advance()) return true;
            case Token::TokenType::U16: if(advance()) return true;
            case Token::TokenType::I32: if(advance()) return true;
            case Token::TokenType::U32: if(advance()) return true;
            case Token::TokenType::I64: if(advance()) return true;
            case Token::TokenType::U64: if(advance()) return true;
        }

        return false;
    }

    std::expected<AST::AST, Diag::Diagnostic> Parser::parse_type()
    {
        if(!expect_type())
        {
            Diag::DiagnosticBuilder diagnostic_builder;

            diagnostic_builder
                .id(Diag::DiagnosticID::Auto)
                .severity(Diag::Severity::Error)
                .primary_location(after(previous().location))
                .add_higlight
                (
                    Diag::Highlight{}
                        .location(after(previous().location))
                        .message
                        (   
                            Diag::Message{}
                                .template_id(Diag::DiagnosticID::ExpectedAfter)
                                .add_argument(Token::pretty_token(previous().type, source_manager.get_string(previous().location)))
                        )
                        .secondary()
                )
                .add_higlight
                (
                    Diag::Highlight{}
                        .location(peek().location)
                        .message
                        (   
                            Diag::Message{}
                                .template_id(Diag::DiagnosticID::Auto)
                                .add_argument(std::string("<type>"))
                                .add_argument(Token::pretty_token(peek().type, source_manager.get_string(peek().location)))
                        )
                        .primary()
                );
                //.add_secondary_location(peek().location)
                //.add_argument(std::string_view("<type>"))
                //.add_argument(Token::pretty_token(peek().type, source_manager.get_string(peek().location)));

            return std::unexpected(diagnostic_builder.build());
        }

        return AST::AST{};
    }

    std::expected<AST::AST, Diag::Diagnostic> Parser::parse_expr()
    {
        if(!expect(Token::TokenType::IntegerLiteral))
        {
            Diag::DiagnosticBuilder diagnostic_builder;

            diagnostic_builder
                .id(Diag::DiagnosticID::Auto)
                .severity(Diag::Severity::Error)
                .primary_location(peek().location)
                .add_higlight
                (
                    Diag::Highlight{}
                        .location(after(previous().location))
                        .message
                        (   
                            Diag::Message{}
                                .template_id(Diag::DiagnosticID::Auto)
                                .add_argument(std::string("<expr>"))
                                .add_argument(Token::pretty_token(peek().type, source_manager.get_string(peek().location)))
                        )
                        .primary()
                );
                //.add_argument(std::string_view("<expr>"))
                //.add_argument(Token::pretty_token(peek().type, source_manager.get_string(peek().location)));
        
            return std::unexpected(diagnostic_builder.build());
        }

        return AST::AST{};
    }



    std::expected<AST::AST, std::vector<Diag::Diagnostic>> Parser::parse_let_dec()
    {
        std::vector<Diag::Diagnostic> diagnostics;

        advance(); //* starts at let, so we try to advance

        if(!expect(Token::TokenType::Identifier))
        {
            Diag::DiagnosticBuilder diagnostic_builder;

            diagnostic_builder
                .id(Diag::DiagnosticID::Auto)
                .severity(Diag::Severity::Error)
                .primary_location(peek().location)
                .add_higlight
                (
                    Diag::Highlight{}
                        .location(peek().location)
                        .message
                        (   
                            Diag::Message{}
                                .template_id(Diag::DiagnosticID::Auto)
                                .add_argument(Token::token_type_name(Token::TokenType::Identifier))
                                .add_argument(Token::pretty_token(peek().type, source_manager.get_string(peek().location)))
                        )
                        .primary()
                );

            if(peek().type != Token::TokenType::Colon && peek().type != Token::TokenType::Equal)
            {
                diagnostic_builder
                    .add_hint
                    (
                        Diag::ReplaceHint
                        {
                            .location=peek().location,
                            .replace=Token::token_type_string(Token::TokenType::Identifier)
                        }
                    );

                while //* recovery
                (
                    peek().type != Token::TokenType::Colon &&
                    peek().type != Token::TokenType::Equal
                    
                )
                { 
                    if(!advance()) break;
                }

            }

            else
                diagnostic_builder
                    .add_hint
                    (
                        Diag::AddHint
                        {
                            .message=Diag::Message{}
                                .template_id(Diag::DiagnosticID::MaybeInsert)
                                .add_argument(Token::token_type_name(Token::TokenType::Identifier)),
                            .location=after(previous().location),
                            .add=Token::token_type_string(Token::TokenType::Identifier)
                        }
                    );

            diagnostics.push_back(diagnostic_builder.build());
        }

        if(expect(Token::TokenType::Colon))
        {
            auto result = parse_type();

            if(!result)
            {
                Diag::DiagnosticBuilder diagnostic_builder(std::move(result.error()));

                if(peek().type != Token::TokenType::Equal)
                {
                    diagnostic_builder
                        .add_hint
                        (
                            Diag::ReplaceHint
                            {
                                .message=Diag::Message{}
                                    .template_id(Diag::DiagnosticID::MaybeReplace)
                                    .add_argument(Token::pretty_token(peek().type, source_manager.get_string(peek().location)))
                                    .add_argument(std::string("<type>")),
                                .location=peek().location,
                                .replace="<type>"
                            }
                        );
                }
                else
                {
                    diagnostic_builder
                        .add_hint
                        (
                            Diag::AddHint
                            {
                                .message=Diag::Message{}
                                    .template_id(Diag::DiagnosticID::MaybeInsert)
                                    .add_argument(std::string("<type>")),
                                .location=after(previous().location),
                                .add="<type>"
                            }
                        );  
                }

                diagnostics.push_back(diagnostic_builder.build());

                if(!advance()) return std::unexpected(diagnostics); //* recovery
            }
        }

        if(!expect(Token::TokenType::Equal))
        {
            Diag::DiagnosticBuilder diagnostic_builder;

            diagnostic_builder
                .id(Diag::DiagnosticID::Auto)
                .severity(Diag::Severity::Error)
                .primary_location(peek().location)
                .add_higlight
                (
                    Diag::Highlight{}
                        .location(peek().location)
                        .message
                        (   
                            Diag::Message{}
                                .template_id(Diag::DiagnosticID::Auto)
                                .add_argument(Token::token_type_name(Token::TokenType::Equal))
                                .add_argument(Token::pretty_token(peek().type, source_manager.get_string(peek().location)))
                        )
                        .primary()
                )
                .add_hint
                (
                    Diag::ReplaceHint
                    {
                        .message=Diag::Message{}
                            .template_id(Diag::DiagnosticID::MaybeReplace)
                            .add_argument(Token::pretty_token(peek().type, source_manager.get_string(peek().location)))
                            .add_argument(Token::token_type_name(Token::TokenType::Equal)),
                        .location=peek().location,
                        .replace=Token::token_type_string(Token::TokenType::Equal)
                    }
                );

            diagnostics.push_back(diagnostic_builder.build());
        }

        if(!diagnostics.empty()) return std::unexpected(diagnostics);

        return AST::AST{};
    }

    std::optional<AST::AST> Parser::parse()
    {
        if(peek().type == Token::TokenType::Let)
        {
            auto result = parse_let_dec();

            if(result) return *result;

            for(auto& diag : result.error()) diag_engine.report(std::move(diag));
            return std::nullopt;
        }

        return std::nullopt;
    }
}