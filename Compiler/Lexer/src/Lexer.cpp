#include "../include/Lexer.hpp"
#include "../../Support/SourceManager/SourceManager.hpp"

namespace Lexer
{
    /*
    char peek() const;
    bool match(const char c) const;
    char advance();
    */
   char Lexer::peek() const
   {
        return source[offset+1];
   }
   bool Lexer::match(const char c) const
   {
        if(source[offset] == c) return true;
        return false;
   }
}