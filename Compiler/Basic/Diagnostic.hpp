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
        {Severity::Note, "note"},
        {Severity::Warning, "warning"},
        {Severity::Error, "error"},
        {Severity::Fatal, "fatal"}
    };

    enum class DiagnosticID
    {
        InvalidCharacter
    };

    struct ReplaceHint
    {
        SourceManager::SourceLocation location;
        std::string replace;
    };

    using FixItHint = std::variant<ReplaceHint>;

    using DiagnosticArgument = std::variant<
        std::string,
        char,
        int,
        uint32_t
    >;

    struct Diagnostic
    {
        DiagnosticID diagnostic_id;
        Severity severity;

        SourceManager::SourceLocation location; // Primary Location

        std::vector<DiagnosticArgument> arguments;

        std::vector<FixItHint> hints;
    };

    constexpr std::array<std::string_view, 3> diagnostic_templates =
    {
        "unexpected character '%' found.",
    };

    class DiagnosticBuilder
    {
        private:
            Diagnostic diagnostic;    

        public:
            DiagnosticBuilder& id(DiagnosticID id);

            DiagnosticBuilder& severity(Severity severity);

            DiagnosticBuilder& primary_location(SourceManager::SourceLocation location);

            DiagnosticBuilder& add_hint(FixItHint hint);

            DiagnosticBuilder& add_argument(const std::string& argument);
            DiagnosticBuilder& add_argument(char argument);

            const Diagnostic& build();
    };
    
    class DiagnosticRenderer
    {
        private:
            SourceManager::SourceManager& source_manager;
            Diagnostic& diagnostic;

        public:
            DiagnosticRenderer(SourceManager::SourceManager& source_manager, Diagnostic& diagnostic)
                : source_manager(source_manager), diagnostic(diagnostic)
            {}

            void render() const;
    };
}