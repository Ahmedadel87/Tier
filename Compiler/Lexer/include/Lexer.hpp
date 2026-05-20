#pragma once

#include <filesystem>
#include <iostream>
#include "../../Basic/Token.h"
#include "../../Support/SourceManager/SourceManager.hpp"

namespace fs = std::filesystem;

namespace Lexer {

    class Lexer 
    {
        private:
            const SourceManager::SourceManager& source_manager;
            SourceManager::FileID file_id;
            std::string source;

            uint32_t line;
            uint32_t column;
            uint32_t offset;

            char peek() const;
            bool match(const char c) const;
            char advance();

        public:
            Lexer(const SourceManager::SourceManager& source_manager, SourceManager::FileID file_id) : 
                source_manager(source_manager) , file_id(file_id) 
            {
                source = source_manager.get_file(file_id).contents;
            }

            Token::Token next_token();
    };

}