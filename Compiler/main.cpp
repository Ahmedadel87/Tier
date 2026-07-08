#include "Lexer/include/Lexer.hpp"
#include "Support/SourceManager/SourceManager.hpp"
#include "Basic/Token.hpp"
#include "Basic/Diagnostic.hpp"
#include <chrono>
#include <fstream>

std::ofstream write("let_dec.tier");

void dump_token(Token::Token& tok)
{
    write << static_cast<uint16_t>(tok.type) << '|' << tok.location.file_id << '|' << tok.location.offset << '|' << tok.location.length << '\n';
}

int main(int argc, char** argv){
    // std::cout << "ENTER MAIN\n";

    if(argc < 2)
    {
        std::cerr << "No path provided.";
        exit(1);
    }
    fs::path path = argv[1];
    if(!fs::exists(path))
    {
        std::cerr << "Path doesn't exist.";
        exit(1);
    }

    SourceManager::SourceManager source_manager;
    SourceManager::FileID file_id = source_manager.add_file(path);

    Lexer::Lexer lex(source_manager, file_id);

    auto start = std::chrono::steady_clock::now();


    while(true)
    {
        auto result = lex.next_token();
        
        if(result)
        {
            Token::Token token = *result;

            if(token.type == Token::TokenType::EoF) break;
            dump_token(*result);
            // std::cout << "Token(" << sizeof(token) << " bytes)\n   L-> type: " << Lexer::out_keywords.at(token.type) << "(" << sizeof(token.type) << " bytes)\n   L-> offset: " << token.location.offset << "(" << sizeof(token.location.offset) <<" bytes)\n   L-> length: " << token.location.length << "(" << sizeof(token.location.length) << " bytes)\n   L-> file id: " << token.location.file_id << "(" << sizeof(token.location.file_id) << " bytes)\n   Obtained:\n      L-> lexeme: " << source_manager.get_string(token.location) << "\n      L-> line: " << source_manager.get_line_column(token.location).first << "\n      L-> column: " << source_manager.get_line_column(token.location).second << "\n\n";
        }
        else 
        {
            Diag::DiagnosticRenderer renderer(source_manager, result.error());
            renderer.render();
            
            break;
        }

    }

    auto end = std::chrono::steady_clock::now();

    double seconds =
        std::chrono::duration<double>(end - start).count();

    std::cout << seconds;

    return 0;
}