#include <cstdint>
#include <iostream>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// TODO: parser, evaluator & repl

enum TokenKind {
    NUMBER,
    PLUS,
    MINUS,
    ILLEGAL,
    END_OF_LINE,
};

struct Token {
    TokenKind kind;
    std::variant<int64_t, char> value;
};

struct Node {
    Token internal;
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
        case '5': case '6': case '7': case '8': case '9': {
            return Token{NUMBER, extract_number()};
        }
        case '+':
            input.remove_prefix(1);
            return Token{PLUS, '+'};
        case '-':
            input.remove_prefix(1);
            return Token{MINUS, '-'};
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
    
    if(sign == MINUS) {
        const auto negated_value = -std::get<int64_t>(current_token.value);
        current_token.value = negated_value;
    }

    return Node{current_token, std::nullopt, std::nullopt};
}

// TODO: improve error handling
auto Reader::parse_expression() -> Node {
    Node expression;
    Node* first_operand;
    Node* second_operand;
    Token current_token, internal;

    if(last_node.has_value()) {
        first_operand = last_node.value();
    } else {
        first_operand = new Node{parse_operand()};
    }
    
    current_token = next_token();
    if(current_token.kind != PLUS && current_token.kind != MINUS) {
        std::cerr << "expr: syntax error: expressions must have operators\n";
        exit(1);
    }
    internal = current_token;

    second_operand = new Node{parse_operand()};

    expression.internal = internal;
    expression.left = first_operand;
    expression.right = second_operand;

    if(!input.empty()) {
        last_node = new Node{expression};
        expression = parse_expression();
    }

    return expression;
}

auto eval_from_node(Node node) -> int64_t {
    switch(node.internal.kind) {
    case PLUS:
        return eval_from_node(*node.left.value()) + eval_from_node(*node.right.value());
    case MINUS:
        return eval_from_node(*node.left.value()) - eval_from_node(*node.right.value());
    case NUMBER:
        return std::get<int64_t>(node.internal.value);
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
    case ILLEGAL:
        return std::format("ILLEGAL[{}]", std::get<char>(token.value));;
    case END_OF_LINE:
        return "END_OF_LINE";
    default:
        std::unreachable();
    }
}

auto debug_token_vector(const std::vector<Token>& tokens) -> std::string {
    std::string output = "{";
    
    const char* separator = "";
    for(const Token& token : tokens) {
        output += separator + debug_token(token);
        separator = ", ";
    }

    output += "}";
    return output;
}

auto debug_node(const Node& node) -> std::string {
    auto left = node.left.has_value() ? debug_node(*node.left.value()) : "None";
    auto right = node.right.has_value() ? debug_node(*node.right.value()) : "None";
    return std::format("Node[Internal: {}, Left: {}, Right: {}]", debug_token(node.internal), left, right);
}

auto main() -> int32_t {
    std::string input = "+1 - +1 + -2";

    Reader reader(input);
    Node tree = reader.parse_expression();
    std::cout << std::format("result of {}: {}\n", input, eval_from_node(tree));

    return 0;
}
