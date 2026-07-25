#include "Token.hpp"

std::string Token::pretty_token(TokenType type, std::string_view lexeme)
{
    std::string pretty_token = token_type_name(type);
    pretty_token += ' ';
    pretty_token += '\'';
    pretty_token.append(lexeme);
    pretty_token += '\'';

    return pretty_token;
}
