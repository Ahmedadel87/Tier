#include "Diagnostic.hpp"
#include <iostream>

void align(size_t line){
    std::cout << "       "; for(int i = 0; i < std::to_string(line).size(); ++i) std::cout << ' ';
}

void align_arrow(size_t pos, size_t size, size_t line){
    align(line);
    for(int i = 0; i < pos + size / 2; ++i) std::cout << ' ';
}

void highlight(int pos, int size){
    for(int i = 0; i < pos; ++i) std::cout << ' ';
    for(int i = 0; i < size; ++i) std::cout << '~';
}

void Report(Error error){
    std::cout << "\x1B[1;91m[Error::\x1B[93m" << int(error.code) << "\x1B[91m]\x1B[0m in " << error.file_name << '\n';
    std::cout << to_string(error.code) << '\n';
    std::cout << "    " << error.line << " | " << error.org_line << '\n';
    align(error.line);
    highlight(error.pos, error.size);
    std::cout << '\n';
    align_arrow(error.pos, error.size, error.line);
    std::cout << "╵-> " << to_expect(error.code);
}

int main(){

    Error error = {5, 1, 123, "let x: = 5", "file.cpp", Code::TYPE_MISS};
    Report(error);

    return 0;
}