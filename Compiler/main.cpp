#include "Lexer/include/Lexer.hpp"
#include "Support/SourceManager/SourceManager.hpp"

int main(int argc, char** argv){
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
    source_manager.add_file(path);

    return 0;
}