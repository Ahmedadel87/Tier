#include "../Support/SourceManager/SourceManager.hpp"

namespace Diagnostic
{
    enum class Severity
    {
        Note, //* Supports other diagnostics, adds extra useful information.
        Warning, //* Indicates a potential bug or risky code.
        Error, //* Cannot continue, code does not adhere to language rules.
        Fatal //* Indicates a non-recoverable error, compiler aborts.
    };

    enum class DiagnosticID
    {
        // TODO: fill errors
    };

    struct Diagnostic
    {
        DiagnosticID diagnostic_id;
        Severity severity;
        SourceManager::SourceLocation location;
    };
}