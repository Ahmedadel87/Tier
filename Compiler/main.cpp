#include "Lexer.hpp"
#include "SourceManager.hpp"
#include "Token.hpp"
#include "Diagnostic.hpp"
#include "Parser.hpp"
#include "SemanticAnalysis.hpp"
#include <chrono>
#include <iostream>

int main(int argc, char** argv){

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

    Parser parser(lexer, source_manager, diag_engine);

    AST::Program *program = parser.parse();

    Sema::ScopeBuilder scope_builder(program);

    Sema::Block *sema_program = scope_builder.build_scoped_program();

    if(sema_program->scope->get_type() == Sema::ScopeType::Program)
        std::cout << "Sema program scope type is Program.\n";

    Diag::DiagnosticRenderer renderer(source_manager);

    renderer.render_all(diag_engine.get_all_diagnostics());

    auto end = std::chrono::steady_clock::now();

    double seconds =
        std::chrono::duration<double>(end - start).count();

    std::cout << "\nTime taken: " << seconds << "sec\n";

    return 0;
}