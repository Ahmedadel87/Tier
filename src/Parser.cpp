#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <variant>

#include "Parser.hpp"
#include "Diagnostic.hpp"

bool is_operator(TokenType type){
    if(type == TokenType::ADD || type == TokenType::SUB || type == TokenType::MUL || type == TokenType::DIV) return true;
    else return false;
}

bool is_type(TokenType type){
    if(type == TokenType::I8 || type == TokenType::U8 || type == TokenType::I16 || type == TokenType::U16 || type == TokenType::I32 || type == TokenType::U32 || type == TokenType::I64 || type == TokenType::U64 || type == TokenType::F32 || type == TokenType::F64 || type == TokenType::MUT){
        return true;
    }
    else return false; 
}

void print_expr(const ExprNode& node, int indent){
    std::string pad(indent * 2, ' ');
    if(std::holds_alternative<LiteralExpr>(node.node)){
        std::cout << pad << std::get<LiteralExpr>(node.node).value.org_word << '\n';
    } else {
        const BinaryExpr& bin = std::get<BinaryExpr>(node.node);
        std::cout << pad << bin.op.org_word << '\n';
        print_expr(*bin.left, indent + 1);
        print_expr(*bin.right, indent + 1);
    }
}

ExprNode Parse_expression(const std::vector<Token>& expr){
    static const std::unordered_map<std::string, int> operator_precedence = {
        {"+", 1},
        {"-", 1},
        {"*", 2},
        {"/", 2},
    }; // greater number = greater precedence

    std::vector<Token> output_stack;
    std::vector<Token> operator_stack;
    for(const Token& token : expr){
        if(token.type== TokenType::INT || token.type== TokenType::FLOAT || token.type== TokenType::STR || token.type== TokenType::IDENTIFIER){
            output_stack.push_back(token);
        }
        else if(is_operator(token.type)){

        while(!operator_stack.empty() && 
            operator_stack.back().type != TokenType::LPARA &&
            operator_precedence.at(operator_stack.back().org_word) >= operator_precedence.at(token.org_word)){
            output_stack.push_back(operator_stack.back());
            operator_stack.pop_back();
        }
            operator_stack.push_back(token);
        }
        else if(token.type == TokenType::LPARA){
            operator_stack.push_back(token);
        }
        else if(token.type == TokenType::RPARA){

            while(operator_stack.back().type != TokenType::LPARA){
                output_stack.push_back(operator_stack.back());
                operator_stack.pop_back();
            }
            operator_stack.pop_back();
        }
    }
    while(!operator_stack.empty()){
        output_stack.push_back(operator_stack.back());
        operator_stack.pop_back();
    }

    std::vector<ExprNode> node_stack;

    for(const Token& token : output_stack){
        if(token.type == TokenType::INT || token.type == TokenType::FLOAT || 
        token.type == TokenType::STR || token.type == TokenType::IDENTIFIER){
            node_stack.push_back(ExprNode{LiteralExpr{token}});
        }
        else if(is_operator(token.type)){
            ExprNode right = std::move(node_stack.back()); node_stack.pop_back();
            ExprNode left  = std::move(node_stack.back()); node_stack.pop_back();
            
            node_stack.push_back(ExprNode{BinaryExpr{
                token,
                std::make_unique<ExprNode>(std::move(left)),
                std::make_unique<ExprNode>(std::move(right))
            }});
        }
    }

    ExprNode root = std::move(node_stack.back());
    std::cout << '\n';
    print_expr(root);
    return root;
}

//* `'let' IDENT [':' TYPE] '=' EXPRESSION ';'`
Parser_AST let_dec(Parser& parser){
    Declaration_Node dec_node;
    if(parser.check(TokenType::IDENTIFIER)) dec_node.identifier = parser.consume().org_word;
    else{
        parser.consume();
        Report(Error{parser.current().org_start_pos, parser.current().org_word.size(), parser.current().line_num, parser.line(), 
                        parser.get_file_name(), Code::AUTO});
        return Parser_AST{};
    }

    if(parser.check(TokenType::COLON)){
        if(is_type(parser.consume(2).type)){
            dec_node.type = parser.current().type;
            std::cout << "type: " << parser.current().org_word;
        }
        /*                
        struct Error{
            size_t pos, size, line;
            const std::vector<Token>& org_line;
            std::string file_name;
            Code code;
        };
        */
        else{
            Report(Error{parser.current().org_start_pos, parser.current().org_word.size(), parser.current().line_num, parser.line(), 
                         parser.get_file_name(), Code::TYPE_MISS});
            return Parser_AST{};
        }
    }
    else dec_node.type = TokenType::NULL_;

    if(parser.check(TokenType::EQUAL)) parser.consume();
    else{
        Report(Error{parser.current().org_start_pos, parser.current().org_word.size(), parser.current().line_num, parser.line(), 
                        parser.get_file_name(), Code::AUTO});
        return Parser_AST{};
    }

    ExprNode expr = Parse_expression(parser.left());
    dec_node.expr = std::move(expr);
    return Parser_AST{Parser_AST_Type::DECLARATION, Parser_Node(std::move(dec_node)), parser.line()};
}

Parser_AST set_and_enforce_statements(Parser& parser){
    Parser_AST_Type type;

    bool enforce = parser.check(TokenType::ENFORCE) ? true : false;
    if(!parser.check(TokenType::SET) && !enforce) return Parser_AST{};
    
    parser.consume();

    //* `'#set typegroup' IDENT '=' TYPE(s) ';'` BRANCH
    if(parser.check(TokenType::TYPEGROUP)){ 
        parser.consume(); 
        type = Parser_AST_Type::TYPE_GROUP_DEC; 
        Type_group_Declaration_Node dec_node;

        if(parser.consume().type == TokenType::IDENTIFIER) dec_node.typegroup = parser.current().org_word;
        else return Parser_AST{};

        if(parser.consume().type != TokenType::EQUAL) return Parser_AST{};
        for(Token token : parser.left()) dec_node.types.push_back(token.type);

        std::cout << "enforce: " << (enforce ? "yes\n" : "no\n");
        std::cout << "type: " << int(type) << '\n';
        std::cout << "types assigned: ";
        for(TokenType type : dec_node.types) std::cout << token_word.at(type) << ' ';

        return Parser_AST{type, Parser_Node(dec_node), parser.line()};
    }

    //* `'#set infer' TYPE_GROUP 'to' TYPE ';'` BRANCH
    //* `'#set infer' TYPE_GROUP 'instead' TYPE_GROUP ';'`
    else if(parser.check(TokenType::INFER)){
        parser.consume();
    }
    return Parser_AST{};
}

/*
struct AST_NODE{
    AST_Type type;
    Node node;
    bool iterable;
    bool const_;
    const std::vector<Token>* reference;
};
*/

Parser_AST parser_AST(Parser& parser){
    switch(parser.current().type){
        case TokenType::LET:
            return let_dec(parser);
            break;
        case TokenType::HASHTAG:
            return set_and_enforce_statements(parser);
            break;
    }
}