#include "DiagnosticEngine.hpp"

namespace Diag
{
    void DiagnosticEngine::report(Diagnostic&& diagnostic)
    {
        if(diagnostic.status.report)
        {
            switch(diagnostic.severity)
            {
                case Severity::Fatal:
                    fatal_error_count++;
                    break;
                case Severity::Error:
                    error_count++;
                    break;
                case Severity::Warning:
                    warning_count++;
                    break;
            }

            diagnostics.push_back(std::move(diagnostic));
        }
    }

    const std::vector<Diagnostic>& 
        DiagnosticEngine::get_all_diagnostics() const
    {
        return diagnostics;
    }
}