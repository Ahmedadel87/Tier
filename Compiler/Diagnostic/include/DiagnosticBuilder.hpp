#pragma once

#include "Diagnostic.hpp"

namespace Diag
{
    class DiagnosticBuilder
    {
        private:
            Diag::Diagnostic diagnostic;

        public:
            DiagnosticBuilder()
            {
                diagnostic.status = {.report=true}; // default status
            }

            DiagnosticBuilder(Diag::Diagnostic&& diag)
                : diagnostic(std::move(diag))
            {}

            DiagnosticBuilder& status(Diag::DiagnosticStatus status);

            DiagnosticBuilder& id(Diag::DiagnosticID id);

            DiagnosticBuilder& severity(Diag::Severity severity);

            DiagnosticBuilder& message(Diag::Message message);

            DiagnosticBuilder& add_hint(Diag::FixItHint hint);

            DiagnosticBuilder& begin_location(SourceManager::SourceLocation location);

            DiagnosticBuilder& primary_location(SourceManager::SourceLocation location);

            DiagnosticBuilder& add_higlight(const Highlight& highlight);

            Diagnostic build();
    };
}