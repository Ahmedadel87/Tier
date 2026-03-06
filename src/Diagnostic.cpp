#include "Diagnostic.hpp"
#include <iostream>

void align(const size_t line){
    std::cout << "       "; for(int i = 0; i < std::to_string(line).size(); ++i) std::cout << ' ';
}

void align_arrow(const size_t pos, const size_t size, const size_t line){
    align(line);
    for(int i = 0; i < pos + size / 2; ++i) std::cout << ' ';
}

void highlight(const int pos, const int size){
    for(int i = 0; i < pos; ++i) std::cout << ' ';
    for(int i = 0; i < size; ++i) std::cout << "\x1B[3;94m~\x1B[0m";
}

void display_error_message(const std::vector<Error>& errors){
    for(Error error : errors) std::cout << "    " << to_string(error.code) << '\n';
}

void display_error_code(const std::vector<Error>& errors){
    std::cout << int(errors[0].code);
    for(int i = 1; i < errors.size(); ++i) std::cout << ':' << int(errors[i].code);
}

#define ASCII

void Report(const std::vector<Error>& errors){
    std::cout << "\x1B[1;91m[Error::\x1B[93m"; 
    display_error_code(errors);
    std::cout << "\x1B[91m]\x1B[0m in \x1B[3;96m" << errors[0].file_name << "\x1B[0m\n";
    
    for(Error error : errors){ 
        std::cout << "    \x1B[3;93m" << error.line << "\x1B[0;3m | \x1B[31m" << error.org_line << "\x1B[0m\n";
        align(error.line);
        highlight(error.pos, error.size);
        std::cout << '\n';
        align_arrow(error.pos, error.size, error.line);
        #ifndef ASCII
        std::cout << "\x1B[3;94m╵";
        #else
        std::cout << "\x1B[3;94mL";
        #endif
        std::cout << "->\x1B[0m " << to_expect(error.code) << '\n';
    }
}