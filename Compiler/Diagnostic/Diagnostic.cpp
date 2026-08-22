#include "../Basic/Diagnostic.hpp"

#include <algorithm>
#include <iostream>
#include <iomanip>

namespace Diag
{
    std::string one_of(std::vector<Token::TokenType> tokens)
    {
        if(tokens.size() == 1)
        {
            return Token::token_type_name(tokens[0]);
        }

        std::string str;

        str += Token::token_type_name(tokens[0]);

        for(int i = 1; i < tokens.size()-1; ++i)
        {
            str += ", " + Token::token_type_name(tokens[i]);
        }

        str += " or " + Token::token_type_name(tokens.back());

        return str;
    } 

    std::string pretty_token(Token::TokenType type, std::string_view lexeme)
    {
        std::string pretty_token = token_type_name(type);
        pretty_token += " \'";
        pretty_token = lexeme;
        pretty_token += '\'';

        return pretty_token;
    }

    DiagnosticBuilder& DiagnosticBuilder::status(DiagnosticStatus status)
    {
        diagnostic.status = status;
        return *this;
    }

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


    Diagnostic DiagnosticBuilder::build()
    {
        auto ret = diagnostic;

        diagnostic = Diagnostic{};

        return ret;
    }

    void DiagnosticEngine::report(Diagnostic&& diagnostic)
    {
        if(diagnostic.status.report)
        {
            diagnostics.push_back(std::move(diagnostic));
        }
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

    std::string DiagnosticRenderer::replace(std::string_view x, const std::vector<DiagnosticArgument>& arguements) const
    {
        std::string replacement;

        int arg = 0;
        for(char c : x)
        {
            if(c == '%')
            {
                replacement += "\x1B[33m";
                if(arguements.size() > arg) replacement += argument_to_string(arguements[arg++]);
                else replacement += "<MISSING>";
                replacement += "\x1B[39m"; // TODO: use the ansi code that only clears colour.
            }
            else replacement += c;
        }

        return replacement;
    }

    void DiagnosticRenderer::print_n_spaces(size_t amount) const
    {
        for(int i = 0; i < amount; ++i) std::clog << ' ';
    }

    void DiagnosticRenderer::render_header(const Diagnostic& diagnostic, std::string line_number, size_t column) const
    {
        //* outputs error type and error code
        std::clog << Diag::severity_name.at(diagnostic.severity)
                    << '['  
                    << std::setw(4) << std::setfill('0') << static_cast<uint16_t>(diagnostic.diagnostic_id) 
                    << "]\n";
        
        //* outputs location (file, line, column) 
        print_n_spaces(line_number.size());
        std::clog << "\x1B[93m-->\x1B[0;3m " << source_manager.get_file(diagnostic.primary_location.file_id).path.string() 
                << ':' << line_number << ':' << column + 1 << "\x1B[0m\n";
    }

    void DiagnosticRenderer::print_gutter(std::string& line_number) const
    {
        print_n_spaces(line_number.size());
        std::clog << " \x1B[93m|\x1B[0m ";
    }

    void DiagnosticRenderer::print_gutter_and_line_number(std::string& line_number) const
    {
        std::clog << line_number << " \x1B[93m|\x1B[0m ";
    }

    void DiagnosticRenderer::highlight(size_t column, size_t length, std::string highlight_with, std::string point_with) const
    {
        print_n_spaces(column);

        if(length == 0) std::clog << point_with;
        for(int i = 0; i < length; ++i) std::clog << highlight_with;
    }

    void DiagnosticRenderer::render_add_hint(AddHint hint) const
    {
        std::clog << replace
        (
            diagnostic_templates[static_cast<uint16_t>(hint.message.m_template_id)], 
            hint.message.m_arguments
        ) 
        << "\x1B[0m\n";

        auto [line, column] = source_manager.get_line_column(hint.location);

        // note: we add 1 since internally it is 0-based index

        std::string line_number = std::to_string(line+1);

        print_gutter(line_number); //* note: we pass line number for formatting although it is not renderered

        std::clog << '\n';

        print_gutter_and_line_number(line_number);

        std::string_view error_line = source_manager.get_line(hint.location.file_id, line);

        for(int i = 0; i < error_line.size(); ++i)
        {
            if(i == column) std::clog << "\x1B[32m" << hint.add << "\x1B[0m";
            std::clog << error_line[i];
        }

        if(column == error_line.size()) std::clog << "\x1B[32m" << hint.add << "\x1B[0m";

        std::clog << '\n';
        
        print_gutter(line_number); //* note: we pass line number for formatting although it is not renderered

        highlight(column, hint.add.size(), "\x1B[32m+\x1B[0m");
    }

    void DiagnosticRenderer::render_replace_hint(ReplaceHint hint) const
    {
        std::clog << replace
        (
            diagnostic_templates[static_cast<uint16_t>(hint.message.m_template_id)], 
            hint.message.m_arguments
        ) 
        << "\x1B[0m\n";

        auto [line, column] = source_manager.get_line_column(hint.location);

        // note: we add 1 since internally it is 0-based index

        std::string line_number = std::to_string(line+1);

        print_gutter(line_number); //* note: we pass line number for formatting although it is not renderered

        std::clog << '\n';

        print_gutter_and_line_number(line_number);

        std::string_view error_line = source_manager.get_line(hint.location.file_id, line);

        for(int i = 0; i < error_line.size(); ++i)
        {
            if(i == column) 
            {
                std::clog << "\x1B[31m";

                for(int j = 0; j < hint.location.length; ++j)
                {
                   std::clog << error_line[i++]; 
                }

                std::clog << "\x1B[0m";
            }

            std::clog << error_line[i];
        }

        std::clog << '\n';

        print_gutter(line_number);

        highlight(column, hint.location.length, "\x1B[31;1m-\x1B[0m");

        std::clog << '\n';

        print_gutter_and_line_number(line_number);

        for(int i = 0; i < error_line.size(); ++i)
        {
            if(i == column) 
            {
                std::clog << "\x1B[32m" << hint.replace << "\x1B[0m";
                i = column + hint.location.length;
            }

            std::clog << error_line[i];
        }

        std::clog << '\n';

        print_gutter(line_number);

        highlight(column, hint.replace.size(), "\x1B[32;1m+\x1B[0m");
    }

    void DiagnosticRenderer::render_hint(FixItHint hint) const
    {
        if(auto val = std::get_if<AddHint>(&hint)) render_add_hint(*val);
        else if(auto val = std::get_if<ReplaceHint>(&hint)) render_replace_hint(*val);
    }

    void DiagnosticRenderer::render_all() const
    {
        for(Diagnostic diagnostic : this->diagnostics)
        {
            auto [line, column] = source_manager.get_line_column(diagnostic.primary_location);

            // note: we add 1 since internally it is 0-based index

            std::string line_number = std::to_string(line+1);

            render_header(diagnostic, line_number, column);

            print_gutter(line_number); //* note: we pass line number for formatting although it is not renderered

            std::clog << '\n';

            print_gutter_and_line_number(line_number);

            std::clog << source_manager.get_line(diagnostic.primary_location.file_id, line);

            //* highlights:

            std::sort(diagnostic.highlights.begin(), diagnostic.highlights.end(),
            [](const auto& a, const auto& b)
            {
                return a.m_location.offset < b.m_location.offset;
            });

            if(!diagnostic.highlights.empty())
            {
                std::clog << '\n';

                print_gutter(line_number);

                uint16_t cursor = 0;

                for(int j = 0; j < diagnostic.highlights.size(); ++j)
                {
                    const Highlight& ht = diagnostic.highlights[j];

                    auto [line, column] = source_manager.get_line_column(ht.m_location);
                    
                    if(ht.is_primary() && column >= cursor) 
                        highlight(column - cursor, ht.m_location.length, "\x1B[31;1m~\x1B[0m", "\x1B[31;1m^\x1B[0m");
                    else if(ht.is_secondary() && column >= cursor)
                        highlight(column - cursor, ht.m_location.length, "\x1B[34;1m~\x1B[0m", "\x1B[34;1m^\x1B[0m");  
                    else if(column >= cursor)
                        highlight(column - cursor, ht.m_location.length, "\x1B[36;1m~\x1B[0m", "\x1B[36;1m^\x1B[0m");

                    cursor = column + ht.m_location.length;
                    if(ht.m_location.length == 0) cursor++;
                }
            }

            while(!diagnostic.highlights.empty())
            {
                std::clog << '\n';

                print_gutter(line_number);

                uint16_t cursor = 0;

                for(int j = 0; j < diagnostic.highlights.size() - 1; ++j)
                {
                    const Highlight& ht = diagnostic.highlights[j];

                    auto [line, column] = source_manager.get_line_column(ht.m_location);

                    if(ht.is_primary() && column > cursor) 
                        highlight(column - cursor, 0, "\x1B[31m|\x1B[0m", "\x1B[31m|\x1B[0m");
                    else if(ht.is_secondary() && column > cursor)
                        highlight(column - cursor, 0, "\x1B[34m|\x1B[0m", "\x1B[34m|\x1B[0m");  
                    else if(column > cursor)
                        highlight(column - cursor, 0, "\x1B[36m|\x1B[0m", "\x1B[36m|\x1B[0m");

                    cursor = column + 1;
                }

                const Highlight& ht = diagnostic.highlights.back();

                auto [line, column] = source_manager.get_line_column(ht.m_location);

                if(column > cursor) print_n_spaces(column - cursor);

                if(ht.is_primary())
                    std::clog << "\x1B[31m";
                else if(ht.is_secondary())
                    std::clog << "\x1B[34m";
                else
                    std::clog << "\x1B[36m";

                if(column >= cursor) std::clog << '+';

                std::clog 
                << "\x1B[1m-> \x1B[0;3m";

                if(ht.is_help())
                    std::clog << "\x1B[36;1mhelp: \x1B[0m";

                std::clog << replace
                    (
                        diagnostic_templates[static_cast<uint16_t>(ht.m_message.m_template_id)], 
                        ht.m_message.m_arguments
                    ) 
                << "\x1B[0m";

                diagnostic.highlights.pop_back();
            }
                        
            std::clog << '\n';
            //* outputs all hints;
            for(const FixItHint& hint : diagnostic.hints)
            {
                print_gutter(line_number);
                std::clog << '\n';
                print_n_spaces(line_number.size());
                std::clog << "\x1B[1;36mhelp\x1B[0m: ";
                render_hint(hint);
                std::clog << '\n';
            }

            std::clog << '\n';
        }
    }
}