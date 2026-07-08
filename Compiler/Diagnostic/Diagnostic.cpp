#include "../Basic/Diagnostic.hpp"
#include "../Support/SourceManager/SourceManager.hpp"
#include <iostream>
#include <iomanip>

namespace Diag
{
    DiagnosticBuilder& DiagnosticBuilder::id(DiagnosticID id)
    {
        diagnostic.diagnostic_id = id;
        return *this;
    }

    DiagnosticBuilder& DiagnosticBuilder::severity(Severity severity)
    {
        diagnostic.severity = severity;
        return *this;
    }

    DiagnosticBuilder& DiagnosticBuilder::primary_location(SourceManager::SourceLocation location)
    {
        diagnostic.location = location;
        return *this;
    }

    DiagnosticBuilder& DiagnosticBuilder::add_hint(FixItHint hint)
    {
        diagnostic.hints.push_back(std::move(hint));
        return *this;
    }

    const Diagnostic& DiagnosticBuilder::build()
    {
        return diagnostic;
    }

    DiagnosticBuilder& DiagnosticBuilder::add_argument(const std::string& argument)
    {
        diagnostic.arguments.push_back(argument);
        return *this;
    }

    DiagnosticBuilder& DiagnosticBuilder::add_argument(char argument)
    {
        diagnostic.arguments.push_back(argument);
        return *this;
    }

    std::string argument_to_string(const DiagnosticArgument& arg)
    {
        return std::visit([](const auto& value)->std::string
        {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::same_as<T, std::string>)
            {
                return value;
            }
            else if constexpr (std::same_as<T, char>)
            {
                return std::string(1, value);
            }
            else if constexpr (std::same_as<T, bool>)
            {
                return value ? "true" : "false";
            }
            else
            {
                return std::to_string(value);
            }
        }, arg);
    }

    std::string replace(std::string_view x, std::vector<DiagnosticArgument> arguements)
    {
        std::string replacement;

        int arg = 0;
        for(char c : x)
        {
            if(c == '%')
            {
                replacement += argument_to_string(arguements[arg++]);
            }
            else replacement += c;
        }

        return replacement;
    }

    void DiagnosticRenderer::render() const
    {
        auto [line, column] = source_manager.get_line_column(diagnostic.location);

        std::clog << Diag::severity_name.at(diagnostic.severity) << '['  << std::setw(4) << std::setfill('0') << static_cast<uint16_t>(diagnostic.diagnostic_id) << ']';
        std::clog << "\n-> " << source_manager.get_file(diagnostic.location.file_id).path.string()  
                  << ':' << line << ':' << column << ":\n\n";

        std::clog << replace(Diag::diagnostic_templates[static_cast<uint16_t>(diagnostic.diagnostic_id)], diagnostic.arguments) << '\n';
    }
}