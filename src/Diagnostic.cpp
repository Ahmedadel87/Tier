#include <iostream>
#include <fstream>
#include "Diagnostic.hpp"
#include "Lexer.hpp"

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
                std::cerr << " -> " << expected[int(error.code)];
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
        std::cerr << " -> " << expected[int(error.code)];
        highlight = false;
        align = 0;
    }
    std::cout << "\x1B[0m";
}

void Report(const Error& error){
    std::cerr << "\x1B[1;91m[Error::\x1B[93m"; 
    std::cerr << int(error.code);
    std::cerr << "\x1B[91m]\x1B[0m in \x1B[3;96m" << error.file_name << "\x1B[0m\n";
    std::cerr << error_names[int(error.code)];
    write_line(error);
}
/*
int main(int argc, char** argv){
    std::string file_name = argv[1];
    std::ifstream read(file_name);
    std::string temp;
    std::vector<std::string> lines;
    while(std::getline(read, temp)) lines.push_back(temp);
    for(std::vector<lexical_token> lex : Stream(lines)){
        Error error = Error{lex[2].org_start_pos, lex[2].string.size(), 1, Tokenize(lex), "somefile.txt", Code::TYPE_MISS};
        Report(error);
    }
    
    * struct Error{
    *    size_t pos, size, line;
    *    const std::vector<Token>& org_line;
    *    std::string file_name;
    *    Code code;
    * };
    

    return 0;
}
*/