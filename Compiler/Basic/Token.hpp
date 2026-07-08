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

        // Types
        I8,
        U8,
        I16,
        U16,
        I32,
        U32,
        I64,
        U64,

        // Special
        EoF
    };

    struct Token 
    {
        TokenType type;
        SourceManager::SourceLocation location;
    };
}