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
    auto parse_expression() -> Node;
};

Reader::Reader(const std::string& string) {
    input = string;
}

auto Reader::skip_spaces() -> void {
    while(not input.empty() && std::isspace(input.at(0))) {
        input.remove_prefix(1);
    }
}

auto Reader::extract_number() -> int64_t {
    std::string numeric_value = "";
    while(not input.empty() && std::isdigit(input.at(0))) {
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
            const auto full_number = extract_number();
            return Token{NUMBER, full_number};
        }
        case '+':
            input.remove_prefix(1);
            return Token{PLUS, '+'};
        default:
            input.remove_prefix(1);
            return Token{ILLEGAL, current_character};
    }
}

// TODO: improve error handling
auto Reader::parse_expression() -> Node {
    Node tree;
    Node* first_operand;
    Node* second_operand;
    Token current_token;

    if(last_node.has_value()) {
        first_operand = last_node.value();
    } else {
        current_token = next_token();
        if(current_token.kind != NUMBER) {
            std::cerr << "expr: runtime error: expressions must begin with natural numbers\n";
            exit(1);
        }
        first_operand = new Node{current_token, std::nullopt, std::nullopt};
    }
    
    current_token = next_token();
    if(current_token.kind != PLUS) {
        std::cerr << "expr: runtime error: expressions must have operators\n";
        exit(1);
    }
    tree.internal = current_token;

    current_token = next_token();
    if(current_token.kind != NUMBER) {
        std::cerr << "expr: runtime error: expressions must end with natural numbers\n";
        exit(1);
    }
    second_operand = new Node{current_token, std::nullopt, std::nullopt};

    tree.left = first_operand;
    if(not input.empty()) {
        last_node = second_operand;
        tree.right = new Node{parse_expression()};
    } else {
        tree.right = second_operand;
    }

    return tree;
}

auto eval_from_node(Node node) -> int64_t {
    switch(node.internal.kind) {
    case PLUS:
        return eval_from_node(*node.left.value()) + eval_from_node(*node.right.value());
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
    std::string input = "1000000000 + 100000000 + 10000000 + 1000000 + 100000 + 10000 + 1000 + 100 + 10 + 1";

    Reader reader(input);
    Node tree = reader.parse_expression();
    std::cout << std::format("result of {}: {}\n", input, eval_from_node(tree));

    return 0;
}
