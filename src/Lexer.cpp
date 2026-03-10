#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Lexer.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <string_view>

is_num_is_decimal isdigitstr(std::string str){
    bool is_decimal = false;
    int dot = 0;
    if(str == "-") return is_num_is_decimal{false, is_decimal};;
    if(str[0] == '-' || std::isdigit(str[0])){
        for(int i = 1; i < str.size(); ++i){
            if(std::isdigit(str[i])){
                continue;
            }
            else if(str[i] == '.' && dot++ == 0){
                is_decimal = true;
                continue;
            }
            else{
                return is_num_is_decimal{false, is_decimal};
            }
        }
        return is_num_is_decimal{true, is_decimal};;
    }
    else{
        return is_num_is_decimal{false, is_decimal};;
    }
}

bool only_space(std::string str){
    if(str.empty()) return true;
    for(char c : str){
        if(c != ' '){
            return false;
        }
    }
    return true;
}

std::string trim(std::string& s) {
    // Trim trailing spaces.
    s.erase(std::find_if_not(s.rbegin(), s.rend(), [](unsigned char c){ return std::isspace(c); }).base(), s.end());
    
    // Trim leading spaces.
    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), [](unsigned char c){ return std::isspace(c); }));
    return s;
}

std::vector<std::vector<lexical_token>> Stream(std::vector<std::string> lines){
    size_t line_num = 0;
    std::vector<lexical_token> strings;
    std::string hold = "";
    size_t i = 0;
    size_t last_first_pos = 0;
    bool ignore = false;   // whether to ignore things like quotes, because of backslashes.
    bool quote1 = false;  // this refers to this quote (').
    bool quote2 = false; // this refers to this quote (").
    bool did;
    for(std::string line : lines){
        bool is_string = false;
        did = false;
        line_num++;
        i = -1;
        last_first_pos = 0;
        trim(line);
        line += ' ';
        //std::cout << ':' << line << ":\n";
        for(char c : line){
            if(!hold.empty() && !only_space(hold) && !did){ 
                last_first_pos = i;
                did = true;
            };
            if(c == '\\') ignore = (ignore) ? false : true;
            else if(c == '\'' && !ignore && !quote2){ 
                quote1 = (quote1) ? false : true;
                is_string = true;
            }
            else if(c == '\"' && !ignore && !quote1){ 
                quote2 = (quote2) ? false : true;
                is_string = true;
            }
            else if((c == ':' || c == ';' || c == '{' || c == '}' || c == '=' || c == '(' || c == ')' || c == '+' || c == '-' || c == '*' || c == '/') && (!quote1 && !quote2)){ // this (if) condition specifies seperators whilst not sperating when within quotes and keeps seperator.
                if(!hold.empty() && !only_space(hold)){  
                    strings.push_back({line_num, trim(hold), last_first_pos, is_string, line});
                    is_string = false;
                    hold.clear();
                }
                hold = c;
                strings.push_back({line_num, trim(hold), i+1, false, line});
                hold.clear();
                did = false;
            }
            else if((c == ' ' || c == ',') && (!quote1 && !quote2) && !hold.empty() && !only_space(hold)){ // this (if) condition specifies seperators when not sperating whilst within quotes.
                strings.push_back({line_num, trim(hold), last_first_pos, is_string, line});
                is_string = false;
                hold.clear();
                did = false;
            }
            else{
                hold += c;
            }
            i++;
        }
    }
    i = 0;
    int last_i = 0;
    int size = strings.size();
    std::vector<std::vector<lexical_token>> complete_lexical_tokens;
    std::vector<lexical_token> temp;
    bool next = false;
    bool hold_i;
    for(lexical_token str : strings){
        if(str.string == ";" || str.string == "{" || str.string == "}"){
            for(int j = last_i; j < i; ++j){
                temp.push_back(strings[j]);            }
            if(str.string == "{") temp.push_back(strings[i]); 
            complete_lexical_tokens.push_back(temp);
            temp.clear();
            if(str.string == "}"){ 
                temp.push_back(strings[i]);
                complete_lexical_tokens.push_back(temp);
                temp.clear();
            }
            if(i+1 <= size) last_i = i+1;
        }
        ++i;
    }
    if(last_i < i){
        for(int j = last_i; j < i; ++j){
            if(!strings[j].string.empty() && !only_space(strings[j].string)) temp.push_back(strings[j]);
        }
        if(!temp.empty()) complete_lexical_tokens.push_back(temp);
    }
    return complete_lexical_tokens;
}

//*- ^-- Produces exact groupping required whilst retaining orginal lines.

std::vector<Token> Tokenize(std::vector<lexical_token> line){
    std::vector<Token> tokens;
    for(lexical_token& token : line){
        std::string org_word = token.string;
        std::string org_line = token.org_line;
        auto it = keywords.find(token.string);
        TokenType type;
        if(token.is_string) type = TokenType::STR;
        else if(it != keywords.end()) type = it->second;
        else if(isdigitstr(token.string).valid){ 
            if(isdigitstr(token.string).decimal) type = TokenType::FLOAT;
            else type = TokenType::INT;
        }
        else type = TokenType::IDENTIFIER;
        tokens.push_back(Token{token.org_start_pos, token.line, org_word, org_line, type});
    }

    return tokens;
}

//* ^-- Produces Token with their types whilst carrying previous information from the lexical tokens (produced in 'Stream' function)