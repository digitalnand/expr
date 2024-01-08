#include <cstdint>
#include <iostream>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

enum TokenKind {
    NUMBER,
    PLUS,
    MINUS,
    TIMES,
    DIVIDED_BY,
    ILLEGAL,
    END_OF_LINE
};

enum NodeKind {
    OPERAND,
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    NEGATION
};

struct Token {
    TokenKind kind;
    std::variant<int64_t, char> value;
};

struct Node {
    NodeKind kind;

    std::optional<int64_t> data;
    std::optional<Node*> left;
    std::optional<Node*> right;
};

struct Reader {
    std::string_view input;
    std::optional<Node*> last_node;

    Reader(const std::string& string);
    auto skip_spaces() -> void;
    auto extract_number() -> int64_t;
    auto next_token() -> Token;

    auto parse_operand() -> Node;
    auto parse_expression() -> Node;
    auto parse_input() -> Node;
};

Reader::Reader(const std::string& string) {
    input = string;
}

auto Reader::skip_spaces() -> void {
    while(!input.empty() && std::isspace(input.at(0))) {
        input.remove_prefix(1);
    }
}

auto Reader::extract_number() -> int64_t {
    std::string numeric_value = "";

    while(!input.empty() && std::isdigit(input.at(0))) {
        numeric_value += input.at(0);
        input.remove_prefix(1);
    }

    return std::stol(numeric_value);
}

auto Reader::next_token() -> Token {
    skip_spaces();

    if(input.empty()) {
        return Token{END_OF_LINE, NULL};
    }

    Token token;
    const auto current_character = input.at(0);

    switch(current_character) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return Token{NUMBER, extract_number()};
        case '+':
            input.remove_prefix(1);
            return Token{PLUS, '+'};
        case '-':
            input.remove_prefix(1);
            return Token{MINUS, '-'};
        case '*':
            input.remove_prefix(1);
            return Token{TIMES, '*'};
        case '/':
            input.remove_prefix(1);
            return Token{DIVIDED_BY, '/'};
        default:
            input.remove_prefix(1);
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
    
    const auto value = std::get<int64_t>(current_token.value);

    if(sign == MINUS) {
        Node expression;
        expression.kind = NEGATION;
        expression.left = std::nullopt;
        expression.right = new Node{OPERAND, value, std::nullopt, std::nullopt};
        return expression;
    }

    return Node{OPERAND, value, std::nullopt, std::nullopt};
}

// TODO: improve error handling
auto Reader::parse_expression() -> Node {
    Node expression;
    Node* first_operand;
    Node* second_operand;
    Token current_token;

    if(last_node.has_value()) {
        first_operand = last_node.value();
    } else {
        first_operand = new Node{parse_operand()};
    }
    
    current_token = next_token();

    switch(current_token.kind) {
        case PLUS:
            expression.kind = ADDITION;
            break;
        case MINUS:
            expression.kind = SUBTRACTION;
            break;
        case TIMES:
            expression.kind = MULTIPLICATION;
            break;
        case DIVIDED_BY:
            expression.kind = DIVISION;
            break;
        default:
            std::cerr << "expr: syntax error: expressions must have operators\n";
            exit(1);
    }

    second_operand = new Node{parse_operand()};

    expression.left = first_operand;
    expression.right = second_operand;

    if(!input.empty()) {
        last_node = new Node{expression};
        expression = parse_expression();
    }

    return expression;
}

auto Reader::parse_input() -> Node {
    Node tree = parse_operand();

    if(!input.empty()) {
        last_node = new Node{tree};
        tree = parse_expression();
    }

    return tree;
}

auto eval_from_node(Node node) -> int64_t {
    switch(node.kind) {
        case ADDITION:
            return eval_from_node(*node.left.value()) + eval_from_node(*node.right.value());
        case SUBTRACTION:
            return eval_from_node(*node.left.value()) - eval_from_node(*node.right.value());
        case MULTIPLICATION:
            return eval_from_node(*node.left.value()) * eval_from_node(*node.right.value());
        case DIVISION:
            return eval_from_node(*node.left.value()) / eval_from_node(*node.right.value());
        case NEGATION:
            return -eval_from_node(*node.right.value());
        case OPERAND:
            return node.data.value();
        default:
            std::unreachable();
    }
}

auto debug_token(const Token& token) -> std::string {
    switch(token.kind) {
        case NUMBER:
            return std::format("NUMBER[{}]", std::get<int64_t>(token.value));
        case PLUS:
            return "PLUS";
        case MINUS:
            return "MINUS";
        case TIMES:
            return "TIMES";
        case DIVIDED_BY:
            return "DIVIDED_BY";
        case ILLEGAL:
            return std::format("ILLEGAL[{}]", std::get<char>(token.value));;
        case END_OF_LINE:
            return "END_OF_LINE";
        default:
            std::unreachable();
    }
}

auto debug_node_kind(const NodeKind& kind) -> std::string {
    switch(kind) {
        case OPERAND:
            return "OPERAND";
        case ADDITION:
            return "ADDITION";
        case SUBTRACTION:
            return "SUBTRACTION";
        case MULTIPLICATION:
            return "MULTIPLICATION";
        case DIVISION:
            return "DIVISION";
        case NEGATION:
            return "NEGATION";
        default:
            std::unreachable();
    }
}

auto debug_node(const Node& node) -> std::string {
    auto value = node.data.has_value() ? std::to_string(node.data.value()) : "None";
    auto left = node.left.has_value() ? debug_node(*node.left.value()) : "None";
    auto right = node.right.has_value() ? debug_node(*node.right.value()) : "None";

    return std::format("Node[Kind: {}, Value: {}, Left: {}, Right: {}]", debug_node_kind(node.kind), value, left, right);
}

auto main() -> int32_t {
    std::string input;

    while(true) {
        std::cout << "> ";

        if(!std::getline(std::cin, input)) {
            exit(0);
        }

        if(input.empty()) {
            continue;
        }

        Reader reader(input);
        const auto tree = reader.parse_input();
        const auto result = eval_from_node(tree);

        std::cout << std::format("{}\n", result);
    }

    return 0;
}
