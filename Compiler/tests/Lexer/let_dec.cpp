#include "../../Lexer/include/Lexer.hpp"
#include "../../Support/SourceManager/SourceManager.hpp"
#include "../TestFramework.hpp"

int main()
{
    SourceManager::SourceManager source_manager;
    SourceManager::FileID file_id = source_manager.add_file("let_dec.tier");

    Lexer::Lexer lex(source_manager, file_id);

    TEST(Lexer, LetDec)
    {
        auto result = lex.next_token();

        TEST_TOK(result, Token::TokenType::Let, {6, 4, 0});
    }

    RUN_TESTS();
}