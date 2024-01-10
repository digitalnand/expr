#include <cctype>
#include <string>
#include <vector>

#include "lexer.h"

#define lexer_advance() \
        pointer++;
#define is_empty(input) \
        (pointer >= input.size())

Lexer::Lexer(const std::string_view string) {
    input = string;
    pointer = 0;
}

auto Lexer::skip_spaces() -> void {
    while(!is_empty(input) && std::isspace(input.at(pointer))) {
        lexer_advance();
    }
}

auto Lexer::extract_number() -> double {
    std::string numeric_value = "";
    bool decimal_began = false;

    while(!is_empty(input)) {
        const auto current_char = input.at(pointer);

        if(!std::isdigit(current_char) && current_char != '.') break;

        if(current_char == '.') {
            // TODO: throw the proper error in this case
            if(decimal_began) break;

            decimal_began = true;
        }

        numeric_value += current_char;
        lexer_advance();
    }

    return std::stod(numeric_value);
}

auto Lexer::next_token() -> Token {
    skip_spaces();

    if(is_empty(input)) return Token{END_OF_LINE, '\0'};

    const auto current_character = input.at(pointer);

    switch(current_character) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9': return Token{NUMBER, extract_number()};

        case '+': lexer_advance(); return Token{PLUS, '+'};
        case '-': lexer_advance(); return Token{MINUS, '-'};
        case '*': lexer_advance(); return Token{TIMES, '*'};
        case '/': lexer_advance(); return Token{DIVIDED_BY, '/'};
        
        default: lexer_advance(); return Token{ILLEGAL, current_character};
    }
}

auto Lexer::is_input_empty() -> bool {
    return is_empty(input);
}
