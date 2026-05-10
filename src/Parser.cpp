#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <variant>
#include "Parser.hpp"
#include "Diagnostic.hpp"

void output_AST(Parser_AST AST){
    std::cout << "type: " << parser_ast_type_word.at(AST.type) << '\n';
    switch(AST.type){
        case Parser_AST_Type::DECLARATION:
            Declaration_Node node = std::move(std::get<Declaration_Node>(AST.node));
            std::cout << ((node.const_) ? "const " : "") << node.identifier << ' ' << token_word.at(node.type) << '\n';
    }
}

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
    TokenType prev_token = TokenType::NULL_;

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
    if(Options::get().dump_ast){ 
        std::cout << '\n';
        print_expr(root);
    }
    return root;
}

void call_error(Parser& parser,Code code = Code::AUTO, TokenType expected = TokenType::NULL_, TokenType got = TokenType::NULL_){ 
    Report(
           Error{parser.current().org_start_pos, 
           parser.current().org_word.size(), parser.current().line_num, 
           parser.line(), parser.get_file_name(), expected, got, code}
    ); 
}

bool require(TokenType expected, Parser& parser){
    if(parser.peek().type == expected){
        return true;
    }
    return false;
}

//* `'let' IDENT [':' TYPE] '=' EXPRESSION ';'`

Parser_AST let_dec(Parser& parser){
    Declaration_Node dec_node;

    if(!require(TokenType::IDENTIFIER, parser)){ 
        parser.consume();
        call_error(parser, Code::AUTO, TokenType::IDENTIFIER, parser.current().type);
        return {};
    }
    parser.consume();

    dec_node.identifier = parser.current().org_word;

    if(require(TokenType::COLON, parser)){
        parser.consume();

        if(is_type(parser.peek().type)){
            parser.consume();
            dec_node.type = parser.current().type;
        }

        else{
            parser.consume();
            call_error(parser, Code::TYPE_MISS, TokenType::NULL_, parser.current().type);
            return Parser_AST{};
        }
        if(Options::get().dump_ast) std::cout << "type: " << parser.current().org_word;
    }
    else dec_node.type = TokenType::NULL_;

    if(!require(TokenType::EQUAL, parser)){
        parser.consume();
        call_error(parser, Code::AUTO, TokenType::EQUAL, parser.current().type);
        return Parser_AST{};
    }
    parser.consume();

    ExprNode expr = Parse_expression(parser.left());
    dec_node.expr = std::move(expr);

    return Parser_AST{Parser_AST_Type::DECLARATION, Parser_Node(std::move(dec_node)), parser.line()};
}

Parser_AST print_statement(Parser& parser){
    if(!require(TokenType::LPARA, parser)){
        parser.consume();
        call_error(parser, Code::AUTO, TokenType::LPARA, parser.current().type);
        return Parser_AST{};
    }
    if(parser.at(parser.size()-1).type != TokenType::RPARA){
        call_error(parser, Code::AUTO, TokenType::RPARA, parser.at(parser.size()-1).type);
        return Parser_AST{};
    }

    std::vector<Token> tokens_to_be_proccessed_for_expressions = parser.left();
    tokens_to_be_proccessed_for_expressions.pop_back();

    if(tokens_to_be_proccessed_for_expressions.empty()){
        call_error(parser);
        return Parser_AST{};
    }

    ExprNode expr = Parse_expression(tokens_to_be_proccessed_for_expressions);
    Print print_node;

    print_node.expr = std::move(expr);

    return Parser_AST{Parser_AST_Type::PRINT, Parser_Node(std::move(print_node)), parser.line()};
}

Parser_AST parser_AST(Parser& parser){
    switch(parser.current().type){
        case TokenType::LET:
            return let_dec(parser);
            break;

        case TokenType::PRINT:
            return print_statement(parser);
            break;
    }
    return Parser_AST{};
}