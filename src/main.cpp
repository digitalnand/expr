#include <format>
#include <iostream>
#include <utility>

#include "reader.h"

auto eval_from_node(Node node) -> double {
    switch(node.kind) {
        case ADDITION:
            return eval_from_node(*node.left.value()) + eval_from_node(*node.right.value());
        case SUBTRACTION:
            return eval_from_node(*node.left.value()) - eval_from_node(*node.right.value());
        case MULTIPLICATION:
            return eval_from_node(*node.left.value()) * eval_from_node(*node.right.value());
        case DIVISION:
            return eval_from_node(*node.left.value()) / eval_from_node(*node.right.value());
        case UNARY_MINUS:
            return -eval_from_node(*node.right.value());
        case OPERAND:
            return node.data.value();
        default:
            std::unreachable();
    }
}

auto main() -> int32_t {
    std::string input;

    while(true) {
        std::cout << "> ";

        if(!std::getline(std::cin, input)) exit(0);

        if(input.empty()) continue;

        Reader reader(input);
        const auto tree = reader.parse_input();
        const auto result = eval_from_node(tree);

        std::cout << std::format("{}\n", result);
    }

    return 0;
}
