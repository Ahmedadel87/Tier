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
    if(type == TokenType::I8 || type == TokenType::U8 || type == TokenType::I16 || type == TokenType::U16 || type == TokenType::I32 || type == TokenType::U32 || type == TokenType::I64 || type == TokenType::U64 || type == TokenType::F32 || type == TokenType::F64){
        return true;
    }
    else return false; 
}

void print_expr(const ExprNode& node, const std::string& prefix, bool is_right){
    if(std::holds_alternative<LiteralExpr>(node.node)){
        std::cout << prefix << (is_right ? "└── " : "├── ");
        std::cout << std::get<LiteralExpr>(node.node).value.org_word << '\n';
    } else {
        const BinaryExpr& bin = std::get<BinaryExpr>(node.node);
        std::cout << prefix << (is_right ? "└── " : "├── ");
        std::cout << bin.op.org_word << '\n';
        std::string new_prefix = prefix + (is_right ? "    " : "    ");
        print_expr(*bin.left, new_prefix, false);
        print_expr(*bin.right, new_prefix, true);
    }
}

ExprNode Parse_expression(const std::vector<Token>& expr){
    std::unordered_map<std::string, int> operator_precedence; // greater number = greater precedence
    operator_precedence["+"] = 1;
    operator_precedence["-"] = 1;
    operator_precedence["*"] = 2;
    operator_precedence["/"] = 2;
    std::vector<Token> output_stack;
    std::vector<Token> operator_stack;
    for(const Token& token : expr){
        if(token.type== TokenType::INT || token.type== TokenType::FLOAT || token.type== TokenType::STR || token.type== TokenType::IDENTIFIER){
            output_stack.push_back(token);
        }
        else if(is_operator(token.type)){

            while(!operator_stack.empty()){
                if(operator_precedence.at(operator_stack.back().org_word) >= operator_precedence.at(token.org_word)){
                    output_stack.push_back(operator_stack.back());
                    operator_stack.pop_back();
                }
                else{
                    break;
                }
            }
            operator_stack.push_back(token);
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
    return root;
}

AST_NODE let_dec(Parser parser){
    Declaration_Node dec_node;
    if(parser.check(TokenType::IDENTIFIER)) dec_node.identifier = parser.consume().org_word;
    else return AST_NODE{};

    if(parser.check(TokenType::COLON)){
        if(is_type(parser.consume(2).type)){
            dec_node.type = parser.current().type;
            std::cout << "type: " << parser.current().org_word;
        }
        else return AST_NODE{};
    }
    else dec_node.type = TokenType::NULL_;

    if(parser.check(TokenType::EQUAL)) parser.consume();
    else return AST_NODE{};

    ExprNode expr = Parse_expression(parser.left());
    dec_node.expr = std::move(expr);

    return AST_NODE{AST_Type::DECLARATION, Node(std::move(dec_node)), false, false, &parser.line()};
}

AST_NODE set_and_enforce_statements(Parser parser){
    AST_Type type;

    bool enforce = parser.check(TokenType::ENFORCE) ? true : false;
    if(!parser.check(TokenType::SET) && !enforce) return AST_NODE{};
    
    parser.consume();

    //* `'#set typegroup' IDENT '=' TYPE(s) ';'` BRANCH
    if(parser.check(TokenType::TYPEGROUP)){ 
        parser.consume(); 
        type = AST_Type::TYPE_GROUP_DEC; 
        Type_group_Declaration_Node dec_node;

        if(parser.consume().type == TokenType::IDENTIFIER) dec_node.typegroup = parser.current().org_word;
        else return AST_NODE{};

        if(parser.consume().type != TokenType::EQUAL) return AST_NODE{};
        for(Token token : parser.left()) dec_node.types.push_back(token.type);

        std::cout << "enforce: " << (enforce ? "yes\n" : "no\n");
        std::cout << "type: " << int(type) << '\n';
        std::cout << "types assigned: ";
        for(TokenType type : dec_node.types) std::cout << token_word.at(type) << ' ';

        return {type, Node(dec_node), false, false, &parser.line()};
    }

    //* `'#set infer' TYPE_GROUP 'to' TYPE ';'` BRANCH
    //* `'#set infer' TYPE_GROUP 'instead' TYPE_GROUP ';'`
    else if(parser.check(TokenType::INFER)){
        parser.consume();
    }
    return AST_NODE{};
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

void AST(Parser parser){
    switch(parser.current().type){
        case TokenType::LET:
            let_dec(parser);
            break;
        case TokenType::HASHTAG:
            set_and_enforce_statements(parser);
            break;
    }
}

int main(int argc, char** argv){
    std::cout << "Hello from Tier\n\n";

    std::string file_name = argv[1];
    std::ifstream read(file_name);
    std::string temp;
    std::vector<std::string> lines;
    while(std::getline(read, temp)) lines.push_back(temp);
    for(std::vector<lexical_token> lex : Stream(lines)){
        AST(Parser(Tokenize(lex)));
    }
    return 0;
}