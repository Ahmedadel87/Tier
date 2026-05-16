#include <iostream>
#include <fstream>
#include <string>
#include "Diagnostic.hpp"
#include "Lexer.hpp"

void replace_all(std::string& str, const std::string& from, const std::string& to){
    size_t pos = 0;

    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

void write_token(const Token& token, size_t& prev){
    if(token.org_start_pos > prev) for(int i = 0; i < token.org_start_pos - prev; ++i) std::cout << ' ';
    std::cout << token.org_word;
    prev = token.org_start_pos + token.org_word.size();
}

void write_line(Error error){
    size_t prev_line = -1;
    size_t prev = 0;
    size_t align = 0;
    bool highlight = false;
    for(const Token& token : error.org_line){
            
        if(token.line_num == prev_line){
            write_token(token, prev);
        }
        else{
            if(highlight){
                std::cerr << '\n';
                for(int i = 0; i < align + error.pos; ++i) std::cerr << ' ';
                for(int i = 0; i < error.size; ++i) std::cerr << '~';

                std::string error_msg = std::string(to_expect(error.code));

                replace_all(error_msg, "{e}", token_word.at(error.expected));
                replace_all(error_msg, "{g}", token_word.at(error.got));

                std::cerr << " ->\x1B[0m " << error_msg;

                highlight = false;
                align = 0;
            }
            prev = 0;
            prev_line = token.line_num;
            if(error.line != prev_line){ 
                std::cerr << "\x1B[2m";
            }
            else{
                highlight = true;
                align =  std::to_string(token.line_num).size() + 7;
            }
            std::cerr << "\n    \x1B[3;93m" << token.line_num << "\x1B[0m";
            if(error.line != prev_line) std::cerr << "\x1B[2m";
            std::cerr << " | \x1B[3;31m"; 
            if(error.line != prev_line) std::cerr << "\x1B[2m";
            write_token(token, prev);
        }
    }
    if(highlight){
        std::cerr << '\n';
        for(int i = 0; i < align + error.pos; ++i) std::cerr << ' ';
        for(int i = 0; i < error.size; ++i) std::cerr << '~';
        std::string error_msg = std::string(to_expect(error.code));

        replace_all(error_msg, "{e}", token_word.at(error.expected));
        replace_all(error_msg, "{g}", token_word.at(error.got));

        std::cerr << " -> \x1B[0m" << error_msg;        highlight = false;
        align = 0;
    }
    std::cout << "\x1B[0m\n";
}

void Report(const Error& error){
    if(!Options::get().debug_no_error){
        std::cerr << "\x1B[1;91m[Error::\x1B[93m"; 
        std::cerr << int(error.code);
        std::cerr << "\x1B[91m]\x1B[0m in \x1B[3;96m" << error.file_name << "\x1B[0m\n";

        std::string error_msg = std::string(to_string(error.code));

        replace_all(error_msg, "{e}", token_word.at(error.expected));
        replace_all(error_msg, "{g}", token_word.at(error.got));

        std::cerr << error_msg;

        write_line(error);
                
        error_msg = std::string(to_hint(error.code));

        replace_all(error_msg, "{e}", token_word.at(error.expected));
        replace_all(error_msg, "{g}", token_word.at(error.got));

        std::cout << "\x1B[1;3;93m[Hint]: \x1B[0m" << error_msg << "\n\n";
    }
}