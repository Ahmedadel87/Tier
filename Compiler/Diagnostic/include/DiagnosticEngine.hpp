#pragma once

#include "Diagnostic.hpp"

namespace Diag
{
    class DiagnosticEngine
    {
        private:
            std::vector<Diagnostic> diagnostics;
            size_t error_count = 0;
            size_t fatal_error_count = 0;
            size_t warning_count = 0;

        public:
            DiagnosticEngine(){}

            void report(Diagnostic&& diagnostic);

            // bool has_error() const;

            // bool has_fatal() const;

            const std::vector<Diagnostic>& get_all_diagnostics() const;
    };
}