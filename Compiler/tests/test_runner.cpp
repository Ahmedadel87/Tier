#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

struct Token
{
    uint32_t type;
    uint32_t file_id;
    uint32_t offset;
    uint16_t length;

    auto operator<=>(const Token&) const = default;
};

std::vector<std::string_view> split(std::string_view s, char delim)
{
    std::vector<std::string_view> out;

    while (true)
    {
        size_t pos = s.find(delim);

        if (pos == std::string_view::npos)
        {
            out.push_back(s);
            break;
        }

        out.push_back(s.substr(0, pos));
        s.remove_prefix(pos + 1);
    }

    return out;
}

#define log(msg) std::cerr << "line: " << line << "\nerr: " << msg << '\n'

void diag(std::string expect, std::string got, size_t line, bool& fail)
{
    std::vector<std::string_view> exp_p = split(expect, '|');
    std::vector<std::string_view> got_p = split(got, '|');
    Token exp_tok = 
    {
        std::stoul(std::string(exp_p[0])),
        std::stoul(std::string(exp_p[1])),
        std::stoul(std::string(exp_p[2])),
        std::stoul(std::string(exp_p[3])),
    };
    Token got_tok = 
    {
        std::stoul(std::string(got_p[0])),
        std::stoul(std::string(got_p[1])),
        std::stoul(std::string(got_p[2])),
        std::stoul(std::string(got_p[3])),
    };

    if(exp_tok == got_tok) return;

    fail = true;

    std::cerr << "line: " << line << "\nerr:\n";

    if(exp_tok.type != got_tok.type)
    { 
        std::cerr << "- type mismatch:\n";
        std::cerr << "    got: " << got_tok.type << '\n';
        std::cerr << "    expected: " << exp_tok.type << '\n';
    }
    if(exp_tok.file_id != got_tok.file_id)
    { 
        std::cerr << "- file id mismatch:\n";
        std::cerr << "    got: " << got_tok.file_id << '\n';
        std::cerr << "    expected: " << exp_tok.file_id<< '\n';
    }
    if(exp_tok.offset != got_tok.offset)
    { 
        std::cerr << "- offset mismatch:\n";
        std::cerr << "    got: " << got_tok.offset << '\n';
        std::cerr << "    expected: " << exp_tok.offset << '\n';
    }
    if(exp_tok.length != got_tok.length)
    { 
        std::cerr << "- length mistmatch:\n";
        std::cerr << "    got: " << got_tok.length << '\n';
        std::cerr << "    expected: " << exp_tok.length<< '\n';
    }

    std::cerr << '\n';
}

int main()
{
    std::ifstream expected_file("Lexer/let_dec.tok");
    std::ifstream got_file("Lexer/let_dec.tier");

    std::string expected_line;
    std::string got_line;

    size_t line = 1;

    bool failed = false;

    while (true)
    {
        bool is_expected_eof = !static_cast<bool>(std::getline(expected_file, expected_line));
        bool is_got_eof = !static_cast<bool>(std::getline(got_file, got_line));

        if(is_expected_eof && is_got_eof) break;

        if(is_expected_eof != is_got_eof)
        {
            log("files of different length");
            break;
        }

        else if(expected_line != got_line)
        {
            diag(expected_line, got_line, line, failed);
        }

        ++line;
    }

    if(!failed) std::cerr << "passed!";
}