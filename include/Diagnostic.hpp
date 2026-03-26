#pragma once

#include <string>
#include <array>
#include <vector>

#include "Lexer.hpp"

enum class Code{
    AUTO,
    TYPE_MISS
};

struct Error{
    size_t pos, size, line;
    const std::vector<Token>& org_line;
    std::string file_name;
    Code code;
};

constexpr std::array<std::string_view, 4> error_names = {
    "Unexpected error.",
    "A colon was found, expected a type."
};

constexpr std::array<std::string_view, 4> expected = {
    "Error token.",
    "Expected a type before this equal, try removing the colon."
};

constexpr std::string_view to_string(Code t) {
    return error_names[static_cast<size_t>(t)];
}

constexpr std::string_view to_expect(Code t) {
    return expected[static_cast<size_t>(t)];
}

void write_token(const Token& token, size_t& prev);

void write_line(Error error);

void Report(const Error& errors);