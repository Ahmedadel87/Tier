#pragma once

#include "Diagnostic.hpp"

namespace Diag
{
    class DiagnosticRenderer
    {
        private:
            SourceManager::SourceManager& source_manager;

            std::string diagnostic_argument_to_string(const DiagnosticArgument& arg);

            void renderer_diagnostic(const Diagnostic& diagnostic);

            void render_severity(Severity severity);
            void render_id(DiagnosticID id);
            void render_message(const Message& message, bool bold);

            void render_header
            (
                Severity severity, 
                DiagnosticID id, 
                const Message& message, 
                SourceManager::SourceLocation primary_location
            );

            void render_lines(Diagnostic diagnostic);

            void render_highlights(std::vector<Highlight> highlights, uint32_t current_line, uint32_t ident);

            void render_hints(std::vector<FixItHint> hints, uint32_t primary_line_number);

            void render_add_hint(AddHint hint);

            void render_replace_hint(ReplaceHint hint);

        public:
            DiagnosticRenderer(SourceManager::SourceManager& source_manager)
                : source_manager(source_manager)
            {}

            void render_all(const std::vector<Diagnostic>& diagnostics);
    };
}