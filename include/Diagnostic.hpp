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
    TokenType expected;
    TokenType got;
    Code code;
};

constexpr std::array<std::string_view, 4> error_names = {
    "expected \x1b[3;33m{e}\x1B[0m instead got: \x1b[3;33m{g}\x1B[0m.",
    "a \x1b[3;33m{g}\x1B[0m was found, expected a type."
};

constexpr std::array<std::string_view, 4> expected = {
    "\x1b[3;33m{g}\x1B[0m found here instead of \x1b[3;33m{e}\x1B[0m",
    "expected a type before this \x1b[3;33m{g}\x1B[0m."
};

constexpr std::array<std::string_view, 4> hints = {
    "maybe you meant: \x1b[3;33m{e}\x1B[0m.",
    "try removing the colon or adding a type."
};

constexpr std::string_view to_string(Code t) {
    return error_names[static_cast<size_t>(t)];
}

constexpr std::string_view to_expect(Code t) {
    return expected[static_cast<size_t>(t)];
}

constexpr std::string_view to_hint(Code t) {
    return hints[static_cast<size_t>(t)];
}

void write_token(const Token& token, size_t& prev);

void write_line(Error error);

void Report(const Error& errors);