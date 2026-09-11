#pragma once

#include "SourceManager.hpp"
#include "Token.hpp"
#include <variant>
#include <array>

namespace Diag
{
    enum class Severity
    {
        Note, //* Supports other diagnostics, adds extra useful information.
        Warning, //* Indicates a potential bug or risky code.
        Error, //* Cannot continue, code does not adhere to language rules.
        Fatal, //* Indicates a non-recoverable error, compiler aborts.
        Help //* Supports other diagnostics, gives a fix.
    };

    enum class DiagnosticID
    {
        Auto,
        ExpectedAfter,
        InvalidCharacter,
        IncompleteScientificNotation,
        MaybeInsert,
        MaybeReplace,
        WhileRecovery,
        AssumingIsValid,
        Missing,
        ExpectedBefore
    };

    constexpr std::array<std::string_view, 10> diagnostic_templates =
    {
        "expected %, found %.",
        "expected here after %.",
        "unexpected character %.",
        "expected exponent digits after %.",
        "consider inserting %.",
        "consider replacing % with %.",
        "while recovering from %.",
        "assuming % is valid.",
        "missing %.",
        "expected here before %."
    };

    using DiagnosticArgument = std::variant<
        std::string,
        char,
        int,
        uint32_t,
        Token::Token,
        Token::TokenType
    >;

    struct Message
    {
        DiagnosticID m_template_id;
        std::vector<DiagnosticArgument> m_arguments;

        Message& add_argument(const DiagnosticArgument& argument)
        {
            m_arguments.push_back(argument);
            return *this;
        }

        Message& template_id(const DiagnosticID& id)
        {
            m_template_id = id;
            return *this;
        }
    };

    struct ReplaceHint
    {
        Message message;
        SourceManager::SourceLocation begin_location;
        SourceManager::SourceLocation primary_location;
        DiagnosticArgument replace;

        ReplaceHint
        (
            SourceManager::SourceLocation begin_location,  
            SourceManager::SourceLocation primary_location, 
            Message message, DiagnosticArgument replace
        ) : begin_location(begin_location), primary_location(primary_location), message(message), replace(replace)
        {}
    };

    struct AddHint
    {
        Message message;
        SourceManager::SourceLocation begin_location;
        SourceManager::SourceLocation primary_location;;
        DiagnosticArgument add;

        AddHint
        (
            SourceManager::SourceLocation begin_location,  
            SourceManager::SourceLocation primary_location, 
            Message message, DiagnosticArgument add
        ) : begin_location(begin_location), primary_location(primary_location), message(message), add(add)
        {}
    };

    using FixItHint = std::variant<ReplaceHint, AddHint>;

    struct Highlight
    {
        enum class Type
        {
            Primary,
            Secondary,
            Help
        };

        Type type;

        Message m_message;
        SourceManager::SourceLocation m_location;

        Highlight& primary()
        {
            this->type = Type::Primary;
            return *this;
        }        

        Highlight& secondary()
        {
            this->type = Type::Secondary;
            return *this;
        }    

        Highlight& help()
        {
            this->type = Type::Help;
            return *this;
        }

        bool is_primary() const
        {
            return this->type == Type::Primary;
        }        

        bool is_secondary() const
        {
            return this->type == Type::Secondary;
        }    

        bool is_help() const
        {
            return this->type == Type::Help;
        }

        Highlight& message(const Message& message)
        {
            this->m_message = message;
            return *this;
        }    

        Highlight& location(const SourceManager::SourceLocation& location)
        {
            this->m_location = location;
            return *this;
        }    
    };

    struct DiagnosticStatus
    {
        bool report;
    };

    struct Diagnostic
    {
        DiagnosticStatus status;

        DiagnosticID id;

        Severity severity;

        Message message;

        SourceManager::SourceLocation begin_location;

        SourceManager::SourceLocation primary_location;

        std::vector<Highlight> highlights;

        std::vector<FixItHint> hints;
    };
}