#include "Lexer/include/Lexer.hpp"
#include "Support/SourceManager/SourceManager.hpp"
#include "Basic/Token.hpp"
#include "Basic/Diagnostic.hpp"
#include "Parser/include/Parser.hpp"
#include <chrono>
#include <iostream>

void dump_token(Token::Token& tok)
{
    std::clog << static_cast<uint16_t>(tok.type) << '|' << tok.location.file_id << '|' << tok.location.offset << '|' << tok.location.length << '\n';
}

int main(int argc, char** argv){
    // std::cout << "ENTER MAIN\n";

    if(argc < 2)
    {
        std::clog << "No path provided.";
        exit(1);
    }
    fs::path path = argv[1];
    if(!fs::exists(path))
    {
        std::clog << "Path doesn't exist.";
        exit(1);
    }

    SourceManager::SourceManager source_manager;
    SourceManager::FileID file_id = source_manager.add_file(path);

    Diag::DiagnosticEngine diag_engine;

    Lexer::Lexer lexer(source_manager, file_id);

    auto start = std::chrono::steady_clock::now();

    Parser::Parser parser(lexer, source_manager, diag_engine);

    parser.parse();

    Diag::DiagnosticRenderer renderer(diag_engine.get_all_diagnostics(), source_manager);

    renderer.render_all();

    auto end = std::chrono::steady_clock::now();

    double seconds =
        std::chrono::duration<double>(end - start).count();

    std::cout << "\nTime taken: " << seconds << "sec\n";

    return 0;
}