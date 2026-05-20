#pragma once

#include <string_view>
#include "../Support/SourceManager/SourceManager.hpp"

namespace Token
{

    enum class TokenType 
    {
        Let,
        Identifier,
        IntegerLiteral,
        FloatLiteral,
        String
    };

    struct Token 
    {
        TokenType type;
        std::string_view lexeme;
        SourceManager::SourceLocation location;
    };

}