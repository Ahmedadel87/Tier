#include "DiagnosticRenderer.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>

namespace Diag
{

    // note: line and columns are 0-based internally

    void newline()
    {
        std::clog << '\n';
    }

    void print_n_spaces(size_t amount)
    {
        for(int i = 0; i < amount; ++i) std::clog << ' ';
    }

    void print_gutter(int line_number)
    {
        print_n_spaces(std::to_string(line_number).size());
        std::clog << " \x1B[93m|\x1B[0m ";
    }

    void print_gutter_and_line_number(int line_number)
    {
        std::clog << line_number+1 << " \x1B[93m|\x1B[0m ";
    }

    // makes it so u can write
    //  1 |
    // 10 | be aligned
    void print_gutter_and_line_number_indented_by(int line_number, int max_line_number)
    {
        print_n_spaces(std::to_string(max_line_number).size() - std::to_string(line_number).size());
        std::clog << line_number+1 << " \x1B[93m|\x1B[0m ";
    }

    void highlight(size_t column, size_t length, std::string highlight_with, std::string point_with)
    {
        print_n_spaces(column);

        if(length == 0) std::clog << point_with;
        for(int i = 0; i < length; ++i) std::clog << highlight_with;
    }

    void DiagnosticRenderer::
        render_severity(Severity severity)
    {
        switch(severity)
        {
            case Severity::Fatal:
                std::clog << "\x1B[31;1mfatal\x1B[0m";
                return;
            case Severity::Error:
                std::clog << "\x1B[31;1merror\x1B[0m";
                return;
            case Severity::Warning:
                std::clog << "\x1B[33;1mwarning\x1B[0m";
                return;
            case Severity::Note:
                std::clog << "\x1B[32;1mnote\x1B[0m";
                return;  
            case Severity::Help:
                std::clog << "\x1B[36;1mhelp\x1B[0m";
                return;        
        }
    }

    void DiagnosticRenderer::
        render_id(DiagnosticID id)
    {
        std::clog 
            << '['
            << std::setfill('0') 
            << std::setw(4) 
            << static_cast<uint16_t>(id)
            << ']';
    }

    std::string DiagnosticRenderer::
        diagnostic_argument_to_string(const DiagnosticArgument& arg)
    {
        return std::visit([this](const auto& value)->std::string
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
            else if constexpr (std::same_as<T, Token::TokenType>)
            {
                return Token::token_type_name(value);
            }
            else if constexpr (std::same_as<T, Token::Token>)
            {
                std::string builder;
                builder.append(Token::token_type_name(value.type));
                builder.push_back(' ');
                builder.push_back('\'');
                builder.append(source_manager.get_string(value.location));
                builder.push_back('\'');

                return 
                    builder;
            }

            return "";
        }, arg);
    }

    void DiagnosticRenderer::
        render_message(const Message& message, bool bold = false)
    {
        std::string_view message_template = diagnostic_templates[static_cast<uint16_t>(message.m_template_id)];

        if(bold)
            std::clog << "\x1B[1m";

        int arg = 0;
        for(char c : message_template)
        {
            if(c == '%')
            {
                std::clog << "\x1B[33m";
                if(message.m_arguments.size() > arg) std::clog << diagnostic_argument_to_string(message.m_arguments[arg++]);
                else std::clog << "<MISSING>";
                std::clog << "\x1B[39m";
            }
            else std::clog << c;
        }
    }

    void DiagnosticRenderer::
        render_header
        (
            Severity severity, 
            DiagnosticID id, 
            const Message& message, 
            SourceManager::SourceLocation primary_location
        )
    {
        render_severity(severity);

        render_id(id);

        std::clog << ": ";

        render_message(message, true);

        newline();

        auto [line, column] = source_manager.get_line_column(primary_location);

        print_n_spaces(std::to_string(line).size());

        std::clog 
            << "\x1B[93m-->\x1B[0m " 
            << source_manager.get_file(primary_location.file_id).path
            << ':'
            << line+1
            << ':'
            << column+1;
    }

    void DiagnosticRenderer::
        render_lines(Diagnostic diagnostic)
    {
        auto begin_line = source_manager.get_line_column(diagnostic.begin_location).first;
        auto end_line = source_manager.get_line_column(diagnostic.primary_location).first;

        for
        (
            int i = begin_line; 
            i <= end_line; 
            ++i
        )
        {
            print_gutter_and_line_number_indented_by(i, end_line);

            std::clog << source_manager.get_line(diagnostic.primary_location.file_id, i);

            newline();

            render_highlights(diagnostic.highlights, i, end_line);
        }
    }


    // todo: rewrite this garbage
    void DiagnosticRenderer::
        render_highlights(std::vector<Highlight> highlights, uint32_t current_line, uint32_t indent)
    {
        auto sorted_highlights = highlights;

        std::sort(sorted_highlights.begin(), sorted_highlights.end(),
            [](const auto& a, const auto& b)
            {
                return a.m_location.offset < b.m_location.offset;
            });

        auto begin_line = source_manager.get_line_column(sorted_highlights.front().m_location).first;
        auto end_line = source_manager.get_line_column(sorted_highlights.back().m_location).first;

        if(current_line < begin_line || current_line > end_line)
            return;

        uint16_t cursor = 0;

        print_gutter(indent);

        for(int i = 0; i < sorted_highlights.size(); ++i)
        {
            const Highlight& ht = sorted_highlights[i];

            auto [line, column] = source_manager.get_line_column(ht.m_location);

            if(line != current_line)
                continue;

            if(ht.is_primary() && column >= cursor) 
                highlight(column - cursor, ht.m_location.length, "\x1B[31;1m~\x1B[0m", "\x1B[31;1m^\x1B[0m");
            else if(ht.is_secondary() && column >= cursor)
                highlight(column - cursor, ht.m_location.length, "\x1B[34;1m~\x1B[0m", "\x1B[34;1m^\x1B[0m");  
            else if(column >= cursor)
                highlight(column - cursor, ht.m_location.length, "\x1B[36;1m~\x1B[0m", "\x1B[36;1m^\x1B[0m");

            cursor = column + ht.m_location.length;
            if(ht.m_location.length == 0) cursor++;
        }

        while(!sorted_highlights.empty())
            {
                const Highlight& ht = sorted_highlights.back();

                auto [line, column] = source_manager.get_line_column(ht.m_location);

                if(line != current_line)
                {
                    sorted_highlights.pop_back();
                    continue;
                }

                std::clog << '\n';

                print_gutter(indent);

                uint16_t cursor = 0;

                for(int i = 0; i < sorted_highlights.size() - 1; ++i)
                {
                    const Highlight& ht = sorted_highlights[i];

                    auto [line, column] = source_manager.get_line_column(ht.m_location);

                    if(line != current_line)
                        continue;

                    if(ht.is_primary() && column >= cursor) 
                        highlight(column - cursor, 0, "\x1B[31m|\x1B[0m", "\x1B[31m|\x1B[0m");
                    else if(ht.is_secondary() && column >= cursor)
                        highlight(column - cursor, 0, "\x1B[34m|\x1B[0m", "\x1B[34m|\x1B[0m");  
                    else if(column >= cursor)
                        highlight(column - cursor, 0, "\x1B[36m|\x1B[0m", "\x1B[36m|\x1B[0m");

                    cursor = column + 1;
                }

                if(column >= cursor) print_n_spaces(column - cursor);

                if(ht.is_primary())
                    std::clog << "\x1B[31m";
                else if(ht.is_secondary())
                    std::clog << "\x1B[34m";
                else
                    std::clog << "\x1B[36m";

                if(column >= cursor) std::clog << '+';

                std::clog 
                << "\x1B[1m-> \x1B[0m";

                if(ht.is_help())
                    std::clog << "\x1B[36;1mhelp: \x1B[0m";

                render_message(ht.m_message);

                sorted_highlights.pop_back();
            }   

        newline();
    }

    void DiagnosticRenderer::
        render_hints(std::vector<FixItHint> hints, uint32_t primary_line_number)
    {
        for(FixItHint hint : hints)
        {
            print_gutter(primary_line_number);

            newline();

            if(auto val = std::get_if<AddHint>(&hint)) render_add_hint(*val);
            else if(auto val = std::get_if<ReplaceHint>(&hint)) render_replace_hint(*val);
        }
    }

    void DiagnosticRenderer::
        render_add_hint(AddHint hint)
    {
        auto [primary_line_number, primary_column] = source_manager.get_line_column(hint.primary_location);

        std::string add = diagnostic_argument_to_string(hint.add);

        render_header
        (
            Severity::Help, 
            hint.message.m_template_id, 
            hint.message, hint.primary_location
        );

        newline();

        print_gutter(primary_line_number);

        newline();

        auto begin_line = source_manager.get_line_column(hint.begin_location).first;
        auto end_line = primary_line_number;

        for
        (
            int i = begin_line; 
            i < end_line; 
            ++i
        )
        {
            print_gutter_and_line_number_indented_by(i, end_line);

            std::clog << source_manager.get_line(hint.primary_location.file_id, i);

            newline();
        }

        print_gutter_and_line_number(end_line);

        std::string_view error_line = 
        source_manager.get_line(hint.primary_location.file_id, primary_line_number);

        for(int i = 0; i < error_line.size(); ++i)
        {
            if(i == primary_column) std::clog << "\x1B[32m" << add << "\x1B[0m ";
            std::clog << error_line[i];
        }

        if(primary_column == error_line.size()) std::clog << "\x1B[32m" << add << "\x1B[0m";

        newline();

        print_gutter(end_line);

        highlight(primary_column, add.size(), "\x1B[32m+\x1B[0m", "\x1B[32m-\x1B[0m");
    }

    void DiagnosticRenderer::
        render_replace_hint(ReplaceHint hint)
    {
        auto [primary_line_number, primary_column] = source_manager.get_line_column(hint.primary_location);

        render_header
        (
            Severity::Help, 
            hint.message.m_template_id, 
            hint.message, hint.primary_location
        );

        newline();

        print_gutter(primary_line_number);

        newline();

        auto begin_line = source_manager.get_line_column(hint.begin_location).first;
        auto end_line = primary_line_number;

        for
        (
            int i = begin_line; 
            i <= end_line; 
            ++i
        )
        {
            print_gutter_and_line_number_indented_by(i, end_line);

            std::clog << source_manager.get_line(hint.primary_location.file_id, i);

            newline();
        }
    }

    void DiagnosticRenderer::
        renderer_diagnostic(const Diagnostic& diagnostic)
    {
        auto [primary_line_number, primary_column] = source_manager.get_line_column(diagnostic.primary_location);

        render_header(diagnostic.severity, diagnostic.id, diagnostic.message, diagnostic.primary_location);

        newline();

        print_gutter(primary_line_number);

        newline();

        render_lines(diagnostic);

        render_hints(diagnostic.hints, primary_line_number);
    }

    void DiagnosticRenderer::
        render_all(const std::vector<Diagnostic>& diagnostics)
    {
        for(const Diagnostic& diagnostic : diagnostics)
            renderer_diagnostic(diagnostic);
    }
}