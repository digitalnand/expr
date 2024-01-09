#pragma once

#include <optional>
#include <string_view>
#include <variant>

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
    UNARY_MINUS
};

struct Token {
    TokenKind kind;
    std::variant<double, char> value;
};

struct Node {
    NodeKind kind;

    std::optional<double> data;
    std::optional<Node*> left;
    std::optional<Node*> right;
};

struct Reader {
    private:
        std::string_view input;
        std::optional<Node*> last_node;

        auto skip_spaces() -> void;
        auto extract_number() -> double;
        auto next_token() -> Token;

        auto parse_operand() -> Node;
        auto parse_expression() -> Node;

    public:
        Reader(const std::string& string);
        auto parse_input() -> Node;
};
