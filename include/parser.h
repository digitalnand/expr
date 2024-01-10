#pragma once

#include <optional>
#include <string_view>

#include "lexer.h"

enum NodeKind {
    OPERAND,
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    UNARY_MINUS
};

struct Node {
    NodeKind kind;

    std::optional<double> data;
    std::optional<Node*> left;
    std::optional<Node*> right;
};

struct Parser {
    private:
        Lexer lexer;
        std::optional<Node*> last_node;

        auto parse_operand() -> Node;
        auto parse_expression() -> Node;
    public:
        Parser(const std::string_view input) : lexer(input){};
        auto parse_input() -> Node;
};
