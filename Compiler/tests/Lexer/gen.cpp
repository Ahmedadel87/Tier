#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <fstream>

// your enum
enum class TokenType 
{
    Let,

    Plus,
    Equal,
    Subtration,
    Multiplication,
    Division,
    Semicolon,

    Identifier,
    IntegerLiteral,
    FloatLiteral,
    String,

    EoF
};

std::string ident(std::mt19937& rng) {
    static const char* letters = "abcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<int> len(3, 12);
    std::uniform_int_distribution<int> ch(0, 25);

    int L = len(rng);
    std::string s;
    s += letters[ch(rng)];

    for (int i = 1; i < L; i++)
        s += letters[ch(rng)];

    return s;
}

int main() {
    std::mt19937 rng(123);

    std::ofstream write("inp.txt");
    std::ofstream test("let_dec.tok");

    size_t offset = 0;

    auto emit = [&](TokenType type, size_t start, size_t len) {
        test
            << static_cast<uint32_t>(type)
            << "|0|"
            << start << "|"
            << len << "\n";
    };

    for (int i = 0; i < 500; i++) {
        std::string id = ident(rng);
        std::string num1 = std::to_string(rng() % 1000);
        std::string num2 = std::to_string(rng() % 1000);

        // let
        write << "let ";
        emit(TokenType::Let, offset, 3);
        offset += 4;

        write << id;
        emit(TokenType::Identifier, offset, id.size());
        offset += id.size() + 1;

        write << " = ";
        emit(TokenType::Equal, offset, 1);
        offset += 2;

        write << num1;
        emit(TokenType::IntegerLiteral, offset, num1.size());
        offset += num1.size() + 1;

        write << " + ";
        emit(TokenType::Plus, offset, 1);
        offset += 2;

        write << num2;
        emit(TokenType::IntegerLiteral, offset, num2.size());
        offset += num2.size();

        write << ";\n";
        emit(TokenType::Semicolon, offset, 1);
        offset += 1;

        offset += 1; // newline
    }
}