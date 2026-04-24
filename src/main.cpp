#include <iostream>
#include <fstream>
#include <string>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Semantic Analysis.hpp"

#include <Options.hpp>

int main(int argc, char** argv){
    std::cout << "Hello from Tier!\n\n";
    if(argc < 2){
        std::cout << "you must provide the file name.";
        return 1;
    }
    std::string file_name = argv[1];
    std::ifstream read(file_name);
    if(!read.is_open()) {
        std::cerr << "could not open \"" << file_name << "\" file.";
        return 1;
    }

    Options::init(argc, argv);

    std::string temp;
    std::vector<std::string> lines;
    
    while(std::getline(read, temp)) lines.push_back(temp);

    for(const std::vector<lexical_token>& lex : Stream(lines)){
        if(Options::get().debug_status) std::cout << "Start\n\n";

        Parser parser(Tokenize(lex), file_name);
        if(Options::get().debug_status) std::cout << "Parser started\n\n";

        std::vector<Parser_AST> temp;
        Parser_AST temp_AST = parser_AST(parser);
        if(Options::get().debug_status) std::cout << "AST created\n\n";

        temp.push_back(std::move(temp_AST));
        if(Options::get().debug_status) std::cout << "Starting semantic\n\n";

        Semantic_Parser(temp);
        if(Options::get().debug_status) std::cout << "Semantic done\n\n";
    }
    std::cout << "\nEnd!";
    return 0;
}