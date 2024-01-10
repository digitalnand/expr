#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <variant>
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

struct Token {
    TokenKind kind;
    std::variant<double, char> value;
};

struct Lexer {
    private:
        std::string_view input;
        uint32_t pointer;

        auto skip_spaces() -> void;
        auto extract_number() -> double;
    public:
        Lexer(const std::string_view string);
        auto is_input_empty() -> bool;
        auto next_token() -> Token;
};
