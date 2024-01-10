#include <iostream>
#include <map>

#include "parser.h"

std::map<NodeKind, uint16_t> precedence_table {
    {ADDITION, 1},
    {SUBTRACTION, 1},
    {MULTIPLICATION, 2},
    {DIVISION, 2}
};

// TODO: improve error handling
auto Parser::parse_operand() -> Node {
    auto sign = PLUS;
    auto current_token = lexer.next_token();

    if(current_token.kind == PLUS || current_token.kind == MINUS) {
        sign = current_token.kind;
        current_token = lexer.next_token();
    }

    if(current_token.kind != NUMBER) {
        std::cerr << "expr: syntax error: operands must be integers\n";
        exit(1);
    }
    
    const auto value = std::get<double>(current_token.value);

    if(sign == MINUS) {
        Node expression;
        expression.kind = UNARY_MINUS;
        expression.left = std::nullopt;
        expression.right = new Node{OPERAND, value, std::nullopt, std::nullopt};
        return expression;
    }

    return Node{OPERAND, value, std::nullopt, std::nullopt};
}

// TODO: improve error handling
auto Parser::parse_expression() -> Node {
    Node current_expression;
    Node first_operand, second_operand;
    Token current_token;

    first_operand = last_node.has_value() ? last_node.value() : parse_operand();
    
    current_token = lexer.next_token();
    switch(current_token.kind) {
        case PLUS: current_expression.kind = ADDITION; break;
        case MINUS: current_expression.kind = SUBTRACTION; break;
        case TIMES: current_expression.kind = MULTIPLICATION; break;
        case DIVIDED_BY: current_expression.kind = DIVISION; break;
        default:
            std::cerr << "expr: syntax error: expressions must have operators\n";
            exit(1);
    }

    second_operand = parse_operand();

    current_expression.left = new Node{first_operand};
    current_expression.right = new Node{second_operand};

    if(last_node.has_value() && precedence_table.contains(last_node.value().kind)) {
        if(precedence_table[current_expression.kind] > precedence_table[last_node.value().kind]) {
            auto last_expression = last_node.value();

            std::swap(current_expression.kind, last_expression.kind);
            std::swap(current_expression.right, last_expression.left);

            std::swap(last_expression.left, last_expression.right);
            std::swap(current_expression.left, current_expression.right);

            current_expression.right = new Node{last_expression};
        }
    }

    if(!lexer.is_input_empty()) {
        last_node = current_expression;
        current_expression = parse_expression();
    }

    return current_expression;
}

auto Parser::parse_input() -> Node {
    Node tree = parse_operand();

    if(!lexer.is_input_empty()) {
        last_node = tree;
        tree = parse_expression();
    }

    return tree;
}
