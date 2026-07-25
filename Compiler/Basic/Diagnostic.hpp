#pragma once

#include "../Support/SourceManager/SourceManager.hpp"
#include <variant>
#include <array>

namespace Diag
{
    enum class Severity
    {
        Note, //* Supports other diagnostics, adds extra useful information.
        Warning, //* Indicates a potential bug or risky code.
        Error, //* Cannot continue, code does not adhere to language rules.
        Fatal //* Indicates a non-recoverable error, compiler aborts.
    };

    static const std::unordered_map<Severity, std::string_view> severity_name =
    {
        {Severity::Note, "\x1B[32mnote\x1B[0m"},
        {Severity::Warning, "\x1B[33mwarning\x1B[0m"},
        {Severity::Error, "\x1B[31merror\x1B[0m"},
        {Severity::Fatal, "\x1B[31;1mfatal\x1B[0m"}
    };

    enum class DiagnosticID
    {
        Auto,
        ExpectedAfter,
        InvalidCharacter,
        IncompleteScientificNotation,
        MaybeInsert,
        MaybeReplace
    };

    constexpr std::array<std::string_view, 6> diagnostic_templates =
    {
        "expected %, found %.",
        "expected after %.",
        "unexpected character %, found.",
        "expected exponent digits after %.",
        "try inserting %.",
        "maybe replace % with %."
    };

    using DiagnosticArgument = std::variant<
        std::string,
        char,
        int,
        uint32_t
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
        SourceManager::SourceLocation location;
        std::string replace;
    };

    struct AddHint
    {
        Message message;
        SourceManager::SourceLocation location;
        std::string add;
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
            if(this->type == Type::Primary) return true;
            return false;
        }        

        bool is_secondary() const
        {
            if(this->type == Type::Secondary) return true;
            return false;
        }    

        bool is_help() const
        {
            if(this->type == Type::Help) return true;
            return false;
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

        DiagnosticID diagnostic_id;
        Severity severity;

        SourceManager::SourceLocation primary_location;

        std::vector<Highlight> highlights;

        std::vector<FixItHint> hints;
    };

    class DiagnosticBuilder
    {
        private:
            Diagnostic diagnostic;

        public:
            DiagnosticBuilder()
            {
                diagnostic.status = {.report=true}; // default status
            }

            DiagnosticBuilder(Diagnostic&& diag)
            {
                diagnostic = diag;
            }

            DiagnosticBuilder& status(DiagnosticStatus status);

            DiagnosticBuilder& id(DiagnosticID id);

            DiagnosticBuilder& severity(Severity severity);

            DiagnosticBuilder& add_hint(FixItHint hint);

            DiagnosticBuilder& primary_location(SourceManager::SourceLocation location);

            DiagnosticBuilder& add_higlight(const Highlight& highlight);

            Diagnostic build() const;
    };
    
    class DiagnosticEngine
    {
        private:
            std::vector<Diagnostic> diagnostics;

        public:
            DiagnosticEngine(){}

            void report(Diagnostic&& diagnostic);

            const std::vector<Diagnostic>& get_all_diagnostics() const
            {
                return diagnostics;
            }
    };

    class DiagnosticRenderer
    {
        private:
            SourceManager::SourceManager& source_manager;
            const std::vector<Diagnostic>& diagnostics;

            std::string replace(std::string_view x, const std::vector<DiagnosticArgument>& arguments) const;
            void print_n_spaces(size_t amount) const;

            void print_gutter(std::string& line_number) const;
            void print_gutter_and_line_number(std::string& line_number) const;
            void highlight(size_t column, size_t length, std::string highlight_with = "\x1B[31;1m~\x1B[0m", std::string point_with = "\x1B[31;1m^\x1B[0m") const;

            void render_add_hint(AddHint hint) const;
            void render_replace_hint(ReplaceHint hint) const;

            void render_hint(FixItHint hint) const;

            void render_header(const Diagnostic& diagnostic, std::string line_number, size_t column) const;

        public:
            DiagnosticRenderer(const std::vector<Diagnostic>& diagnostics, SourceManager::SourceManager& source_manager)
                : diagnostics(diagnostics), source_manager(source_manager)
            {}

            void render_all() const;
    };
}