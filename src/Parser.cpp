#include <iostream>
#include <string>
#include <fstream>
#include "Parser.hpp"

bool is_type(TokenType type){
    if(type == TokenType::I8 || type == TokenType::U8 || type == TokenType::I16 || type == TokenType::U16 || type == TokenType::I32 || type == TokenType::U32 || type == TokenType::I64 || type == TokenType::U64 || type == TokenType::F32 || type == TokenType::F64){
        return true;
    }
    return false; 
}

bool is_expected(const std::vector<Expected>& expected_vector, const TokenType& have){
    for(Expected expected : expected_vector){
        switch(expected){
            case Expected::COLON:
                if(have == TokenType::COLON) return true;
                break;
            case Expected::TYPE:
                if(is_type(have)) return true;
                break;
            case Expected::IDENTIFIER:
                if(have == TokenType::IDENTIFIER) return true;
                break;
            case Expected::EQUAL:
                if(have == TokenType::EQUAL) return true;
                break;
            case Expected::LITERAL:
                if(have == TokenType::INT || have == TokenType::FLOAT || have == TokenType::STR) return true;
                break;
            case Expected::RPARA:
                if(have == TokenType::RPARA) return true;
                break;
            case Expected::LPARA:
                if(have == TokenType::LPARA) return true;
                break;
            case Expected::START:
                switch(have){
                    case TokenType::LET:
                        return true;
                        break;
                    case TokenType::PRINT:
                        return true;
                        break;
                    default:
                        return true;
                }
                break;
        }
    }
    return false;
}

void REPORT(const std::vector<Expected>& expected_vector, const TokenType& got, std::string file_name){
    std::unordered_map<Expected, std::string> expected_word;
    expected_word[Expected::IDENTIFIER] = "IDENTIFIER";
    expected_word[Expected::VARIABLE] = "VARIABLE";
    expected_word[Expected::COLON] = "COLON";
    expected_word[Expected::TYPE] = "TYPE";
    expected_word[Expected::EQUAL] = "EQUAL";
    expected_word[Expected::LITERAL] = "LITERAL";
    expected_word[Expected::START] = "START";
    std::cout << "\n" << file_name << "[Error::Parser]";
    std::cout << "\nExpected: ";
    for(Expected exp : expected_vector) std::cout << expected_word[exp] << ", ";
    std::cout << "Instead got " << token_word[got] << '\n';
    
}

void LET(const Token& token, AST1_Type& type, std::vector<Expected>& expected_vector){
    type = AST1_Type::DECLARATION;
    expected_vector.clear();
    expected_vector = {Expected::IDENTIFIER};
}

void IDENT(const Token& token, std::string& ident, std::vector<Expected>& expected_vector){
    ident = token.org_word;
    expected_vector.clear();
    expected_vector = {Expected::COLON, Expected::EQUAL};
}   

void COLON(std::vector<Expected>& expected_vector){
    expected_vector.clear();
    expected_vector = {Expected::TYPE, Expected::EQUAL};
}

void TYPE(const Token& token, std::vector<Expected>& expected_vector, TokenType& dec_type){
    expected_vector.clear();
    expected_vector = {Expected::EQUAL};

    dec_type = token.type;
}

void EQUAL(std::vector<Expected>& expected_vector){
    expected_vector.clear();
    expected_vector = {Expected::LITERAL, Expected::IDENTIFIER};
}

void LITERAL(std::vector<Token>& expr, const Token& token){
    expr.push_back(token);
}

void PRINT(const Token& token, AST1_Type& type, std::vector<Expected>& expected_vector){
    type = AST1_Type::PRINT;
    expected_vector.clear();
    expected_vector = {Expected::RPARA};
}

void RPARA(std::vector<Expected>& expected_vector){
    expected_vector.clear();
    expected_vector = {Expected::RPARA};
}

void call_token(std::vector<Token>& expr, const Token& token, AST1_Type& type, std::string& ident, std::vector<Expected>& expected_vector, TokenType& dec_type){
    if(token.type== TokenType::LET) LET(token, type, expected_vector);
    else if(token.type== TokenType::IDENTIFIER) IDENT(token, ident, expected_vector);
    else if(token.type== TokenType::COLON) COLON(expected_vector);
    else if(is_type(token.type)) TYPE(token, expected_vector, dec_type);
    else if(token.type== TokenType::EQUAL) EQUAL(expected_vector);
    else if(token.type== TokenType::INT) LITERAL(expr, token);
    else if(token.type== TokenType::PRINT) PRINT(token, type, expected_vector);
}

#define DEBUG

std::vector<AST1_NODE> AST1(const std::vector<Token>& line, std::string file_name){
    std::vector<Token> expr;
    AST1_Type type;
    TokenType dec_type;
    std::string ident;
    std::vector<Expected> expected_vector = {Expected::START};
    #ifdef DEBUG
    int z = 0;
    #endif
    for(size_t i = 0; i < line.size(); ++i){
        const Token& token = line[i];
        #ifdef DEBUG
        std::cout << token_word[token.type] << ' ';
        #endif
        if(is_expected(expected_vector, token.type)) call_token(expr, token, type, ident, expected_vector, dec_type);
        else REPORT(expected_vector, token.type, file_name);
    }
    #ifdef DEBUG
    std::cout << "\nOUT:\n";
    std::cout << int(type) << ' ';
    std::cout << token_word[dec_type] << ' ';
    std::cout << ident << ' ';
    for(Token x : expr) std::cout << x.org_word << ' ';
    #endif
    return std::vector<AST1_NODE>{};
}

int main(){
    token_word[TokenType::LET] = "LET";
    token_word[TokenType::INT] = "INT";
    token_word[TokenType::FLOAT] = "FLOAT";
    token_word[TokenType::IDENTIFIER] = "IDENTIFIER";
    token_word[TokenType::COLON] = "COLON";
    token_word[TokenType::I8] = "I8";
    token_word[TokenType::U8] = "U8";
    token_word[TokenType::I16] = "I16";
    token_word[TokenType::U16] = "U16";
    token_word[TokenType::I32] = "I32";
    token_word[TokenType::U32] = "U32";
    token_word[TokenType::I64] = "I64";
    token_word[TokenType::U64] = "U64";
    token_word[TokenType::F32] = "F32";
    token_word[TokenType::F64] = "F64";
    token_word[TokenType::LBRAC] = "LBRAC";
    token_word[TokenType::RBRAC] = "RBRAC";
    token_word[TokenType::LPARA] = "LPARA";
    token_word[TokenType::RPARA] = "RPARA";
    token_word[TokenType::EQUAL] = "EQUAL";
    token_word[TokenType::STR] = "STR";
    token_word[TokenType::PRINT] = "PRINT";
    token_word[TokenType::NULL_] = "NULL_";

    std::string file_name = "syntax testing.txt";
    std::ifstream read(file_name);
    std::string temp;
    std::vector<std::string> lines;
    while(std::getline(read, temp)) lines.push_back(temp);
    for(std::vector<lexical_token> lex : Stream(lines)){
        AST1(Tokenize(lex), file_name);
    }
    return 0;
}
