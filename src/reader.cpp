#include <cctype>
#include <iostream>
#include <map>
#include <string>

#include "reader.h"

#define tokenizer_advance(input) \
        input.remove_prefix(1);

std::map<NodeKind, int> precedence_table {
    {ADDITION, 1},
    {SUBTRACTION, 1},
    {MULTIPLICATION, 2},
    {DIVISION, 2}
};

Reader::Reader(const std::string& string) {
    input = string;
}

auto Reader::skip_spaces() -> void {
    while(!input.empty() && std::isspace(input.at(0))) {
        tokenizer_advance(input);
    }
}

auto Reader::extract_number() -> double {
    std::string numeric_value = "";
    bool decimal_began = false;

    while(!input.empty()) {
        const auto current_char = input.at(0);

        if(!std::isdigit(current_char) && current_char != '.') break;

        numeric_value += current_char;
        tokenizer_advance(input);

        if(current_char == '.') {
            // TODO: throw the proper error in this case
            if(decimal_began) break;

            decimal_began = true;
        }
    }

    return std::stod(numeric_value);
}

auto Reader::next_token() -> Token {
    skip_spaces();

    if(input.empty()) return Token{END_OF_LINE, '\0'};

    const auto current_character = input.at(0);

    switch(current_character) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return Token{NUMBER, extract_number()};
        case '+':
            tokenizer_advance(input);
            return Token{PLUS, '+'};
        case '-':
            tokenizer_advance(input);
            return Token{MINUS, '-'};
        case '*':
            tokenizer_advance(input);
            return Token{TIMES, '*'};
        case '/':
            tokenizer_advance(input);
            return Token{DIVIDED_BY, '/'};
        default:
            tokenizer_advance(input);
            return Token{ILLEGAL, current_character};
    }
}

// TODO: improve error handling
auto Reader::parse_operand() -> Node {
    auto sign = PLUS;
    auto current_token = next_token();

    if(current_token.kind == PLUS || current_token.kind == MINUS) {
        sign = current_token.kind;
        current_token = next_token();
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
auto Reader::parse_expression() -> Node {
    Node current_expression;
    Node* first_operand;
    Node* second_operand;
    Token current_token;

    first_operand = last_node.has_value() ? last_node.value() : new Node{parse_operand()};
    
    current_token = next_token();
    switch(current_token.kind) {
        case PLUS: current_expression.kind = ADDITION; break;
        case MINUS: current_expression.kind = SUBTRACTION; break;
        case TIMES: current_expression.kind = MULTIPLICATION; break;
        case DIVIDED_BY: current_expression.kind = DIVISION; break;
        default:
            std::cerr << "expr: syntax error: expressions must have operators\n";
            exit(1);
    }

    second_operand = new Node{parse_operand()};

    current_expression.left = first_operand;
    current_expression.right = second_operand;

    if(last_node.has_value() && precedence_table.contains(last_node.value()->kind)) {
        if(precedence_table[current_expression.kind] > precedence_table[last_node.value()->kind]) {
            auto last_expression = *last_node.value();

            std::swap(current_expression.kind, last_expression.kind);
            std::swap(current_expression.right, last_expression.left);

            std::swap(last_expression.left, last_expression.right);
            std::swap(current_expression.left, current_expression.right);

            current_expression.right = new Node{last_expression};
        }
    }

    if(!input.empty()) {
        last_node = new Node{current_expression};
        current_expression = parse_expression();
    }

    return current_expression;
}

auto Reader::parse_input() -> Node {
    Node tree = parse_operand();

    if(!input.empty()) {
        last_node = new Node{tree};
        tree = parse_expression();
    }

    return tree;
}
