#include "DiagnosticBuilder.hpp"

namespace Diag
{
    DiagnosticBuilder& DiagnosticBuilder::status(DiagnosticStatus status)
    {
        diagnostic.status = status;
        return *this;
    }

    DiagnosticBuilder& DiagnosticBuilder::id(DiagnosticID id)
    {
        diagnostic.id = id;
        return *this;
    }

    DiagnosticBuilder& DiagnosticBuilder::severity(Severity severity)
    {
        diagnostic.severity = severity;
        return *this;
    }

    DiagnosticBuilder& DiagnosticBuilder::
        message(Diag::Message message)
    {
        diagnostic.message = message;
        return *this;
    }

    DiagnosticBuilder& DiagnosticBuilder::add_hint(FixItHint hint)
    {
        diagnostic.hints.push_back(std::move(hint));
        return *this;
    }

    DiagnosticBuilder& DiagnosticBuilder::add_higlight(const Highlight& highlight)
    {
        diagnostic.highlights.push_back(highlight);
        return *this;
    }

    DiagnosticBuilder& DiagnosticBuilder::
        primary_location(SourceManager::SourceLocation location)
    {
        diagnostic.primary_location = location;
        return *this;
    }

    DiagnosticBuilder& DiagnosticBuilder::
        begin_location(SourceManager::SourceLocation location)
    {
        diagnostic.begin_location = location;
        return *this;
    }


    Diagnostic DiagnosticBuilder::build()
    {
        auto ret = diagnostic;

        diagnostic = Diagnostic{};

        return ret;
    }
}