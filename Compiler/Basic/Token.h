#pragma once

#include <string_view>
#include "../Support/SourceManager/SourceManager.hpp"

namespace Token
{

    enum class TokenType 
    {
        // Keywords
        Let,

        // Operators
        Plus,
        Equal,
        Subtration,
        Multiplication,
        Division,
        Semicolon,

        // Literals
        Identifier,
        IntegerLiteral,
        FloatLiteral,
        String,

        // Special
        EoF
    };

    struct Token 
    {
        TokenType type;
        SourceManager::SourceLocation location;
    };
}