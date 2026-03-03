#pragma once

#include <string>
#include <array>

#include "Lexer.hpp"

enum class Code{
    TYPE_MISS
};

struct Error{
    size_t pos, size, line;
    std::string org_line;
    std::string file_name;
    Code code;
};

constexpr std::array<std::string_view, 4> error_names = {
    "A colon was found, expected a type."
};

constexpr std::array<std::string_view, 4> expected = {
    "Type was not found, to infer remove the colon."
};

constexpr std::string_view to_string(Code t) {
    return error_names[static_cast<size_t>(t)];
}

constexpr std::string_view to_expect(Code t) {
    return expected[static_cast<size_t>(t)];
}

void Report(Error error);