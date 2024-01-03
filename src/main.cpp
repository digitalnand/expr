#include <iostream>
#include <format>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>
#include <sstream>

// TODO: parser, evaluator & repl

enum TokenKind {
    NUMBER,
    PLUS,
    ILLEGAL,
    END_OF_LINE,
};

struct Token {
    TokenKind kind;
    std::optional<int> data;
};

struct Lexer {
    std::string_view input;

    Lexer(const std::string& string);
    auto skip_spaces()     -> void;
    auto extract_number()  -> int;
    auto next_token()      -> Token;
    auto lex()             -> std::vector<Token>;
};

Lexer::Lexer(const std::string& string) {
    input = string;
}

auto Lexer::skip_spaces() -> void {
    while(not input.empty() && std::isspace(input.at(0))) {
        input.remove_prefix(1);
    }
}

auto Lexer::extract_number() -> int {
    std::string numeric_value = "";
    while(not input.empty() && std::isdigit(input.at(0))) {
        numeric_value += input.at(0);
        input.remove_prefix(1);
    }
    return std::stoi(numeric_value);
}

auto Lexer::next_token() -> Token {
    skip_spaces();

    if(input.empty()) {
        return Token{END_OF_LINE, std::nullopt};
    }

    Token token;
    auto current_character = input.at(0);

    if(std::isdigit(current_character)) {
        return Token{NUMBER, extract_number()};
    }
    else if(current_character == '+') {
        input.remove_prefix(1);
        return Token{PLUS, std::nullopt};
    }
    else {
        input.remove_prefix(1);
        return Token{ILLEGAL, std::nullopt};
    }
}

auto Lexer::lex() -> std::vector<Token> {
    std::vector<Token> tokens;

    Token token;
    while((token = next_token()).kind != END_OF_LINE) {
        tokens.push_back(token);
    }

    return tokens;
}

auto debug_token(const Token& token) -> std::string {
    switch(token.kind) {
    case NUMBER:
        return std::format("NUMBER[{}]", token.data.value());
    case PLUS:
        return "PLUS";
    case ILLEGAL:
        return "ILLEGAL";
    case END_OF_LINE:
        return "END_OF_LINE";
    default:
        std::unreachable();
    }
}

auto debug_token_vector(const std::vector<Token>& tokens) -> std::string {
    std::ostringstream output;
    output << "{";
    
    const char* separator = "";
    for (const Token& token : tokens) {
        output << separator << debug_token(token);
        separator = ", ";
    }

    output << "}";
    return output.str();
}

auto main() -> int {
    std::string input = "    123456789  +987654321";
    std::cout << std::format("input: {}\n", input);

    Lexer lexer(input);
    std::cout << std::format("tokens: {}\n", debug_token_vector(lexer.lex()));
    return 0;
}
