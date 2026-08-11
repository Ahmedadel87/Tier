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
        Colon,
        LPARA,
        RPARA,

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
        EoF,
        Null
    };

    std::string pretty_token(TokenType type, std::string_view lexeme);

    constexpr std::string token_type_string(TokenType type)
    {
        switch (type)
        {
            case TokenType::Let:                return "let";

            case TokenType::Plus:               return "+";
            case TokenType::Equal:              return "=";
            case TokenType::Subtration:         return "-";
            case TokenType::Multiplication:     return "*";
            case TokenType::Division:           return "/";
            case TokenType::Semicolon:          return ";";
            case TokenType::Colon:              return ":";
            case TokenType::LPARA:              return "(";
            case TokenType::RPARA:              return ")";

            case TokenType::Identifier:         return "<identifier>";
            case TokenType::IntegerLiteral:     return "<integer literal>";
            case TokenType::FloatLiteral:       return "<float literal>";
            case TokenType::String:             return "<string>";

            case TokenType::I8:                 return "i8";
            case TokenType::U8:                 return "u8";
            case TokenType::I16:                return "i16";
            case TokenType::U16:                return "u16";
            case TokenType::I32:                return "i32";
            case TokenType::U32:                return "u32";
            case TokenType::I64:                return "i64";
            case TokenType::U64:                return "u64";

            case TokenType::EoF:                return "<EOF>";
            case TokenType::Null:               return "NUll";
        }

        return "<unknown token>";
    }

    constexpr std::string token_type_name(TokenType type)
    {
        switch (type)
        {
            case TokenType::Let:                return "<kw-let>";

            case TokenType::Plus:               return "<plus>";
            case TokenType::Equal:              return "<equal>";
            case TokenType::Subtration:         return "<minus>";
            case TokenType::Multiplication:     return "<astestrik>";
            case TokenType::Division:           return "<slash>";
            case TokenType::Semicolon:          return "<semicolon>";
            case TokenType::Colon:              return "<colon>";
            case TokenType::LPARA:              return "<lpara>";
            case TokenType::RPARA:              return "<rpara>";

            case TokenType::Identifier:         return "<identifier>";
            case TokenType::IntegerLiteral:     return "<integer literal>";
            case TokenType::FloatLiteral:       return "<float literal>";
            case TokenType::String:             return "<string>";

            case TokenType::I8:                 return "<i8>";
            case TokenType::U8:                 return "<u8>";
            case TokenType::I16:                return "<i16>";
            case TokenType::U16:                return "<u16>";
            case TokenType::I32:                return "<i32>";
            case TokenType::U32:                return "<u32>";
            case TokenType::I64:                return "<i64>";
            case TokenType::U64:                return "<u64>";

            case TokenType::EoF:                return "<EOF>";
            case TokenType::Null:               return "<NUll>";
        }

        return "<unknown token>";
    }

    struct Token 
    {
        TokenType type;
        SourceManager::SourceLocation location;

        bool is(TokenType p_type)
        {
            return type == p_type;
        }
    };
}