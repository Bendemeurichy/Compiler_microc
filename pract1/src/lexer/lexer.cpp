#include "lexer.hpp"

#include "llvm/Support/Debug.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Support/raw_ostream.h"

#include <cstdio>
#include <fmt/core.h>
#include <iostream>
#include <iterator>

#define DEBUG_TYPE "lexer"

Lexer::Lexer(const std::string &input)
    : input(input), begin_location(1, 1), end_location(1, 1) {
    begin = end = std::begin(this->input);
}

std::vector<Token> Lexer::getTokens() {
    while (!isAtEnd()) {
        lexToken();

        begin = end;
        begin_location = end_location;
    }

    return tokens;
}

bool Lexer::hadError() const { return errorFlag; }

bool Lexer::isAtEnd() const { return end == std::end(input); }

void Lexer::lexToken() {
    // ASSIGNMENT: Implement the lexical analyser here.
    char c = peek();
    advance();

    switch (c) {
    // Parse whitespace characters
    case ' ':
        break;
    case '\t':
        break;
    case '\n':
        break;
    case '\r':
        break;
    // Parse single character tokens
    case ',':
        emitToken(TokenType::COMMA);
        break;
    case ';':
        emitToken(TokenType::SEMICOLON);
        break;
    case '(':
        emitToken(TokenType::LEFT_PAREN);
        break;
    case ')':
        emitToken(TokenType::RIGHT_PAREN);
        break;
    case '{':
        emitToken(TokenType::LEFT_BRACE);
        break;
    case '}':
        emitToken(TokenType::RIGHT_BRACE);
        break;
    case '[':
        emitToken(TokenType::LEFT_BRACKET);
        break;
    case ']':
        emitToken(TokenType::RIGHT_BRACKET);
        break;
    case '=':
        if (peek() == '=') {
            advance();
            emitToken(TokenType::EQUALS_EQUALS);
        } else {
            emitToken(TokenType::EQUALS);
        }
        break;
    case '<':
        if (peek() == '=') {
            advance();
            emitToken(TokenType::LESS_THAN_EQUALS);
        } else {
            emitToken(TokenType::LESS_THAN);
        }
        break;
    case '>':
        if (peek() == '=') {
            advance();
            emitToken(TokenType::GREATER_THAN_EQUALS);
        } else {
            emitToken(TokenType::GREATER_THAN);
        }
        break;
    case '+':
        emitToken(TokenType::PLUS);
        break;
    case '-':
        emitToken(TokenType::MINUS);
        break;
    case '*':
        emitToken(TokenType::STAR);
        break;
    case '/':
        if (peek() == '/') {
            while (peek() != '\n' && !isAtEnd()) {
                advance();
            }
            break;
        }
        emitToken(TokenType::SLASH);
        break;
    case '^':
        emitToken(TokenType::CARET);
        break;
    case '%':
        emitToken(TokenType::PERCENT);
        break;
    case '!':
        if (peek() != '=') {
            error("Expected '=' after '!'");
            break;
        }
        advance();
        emitToken(TokenType::BANG_EQUALS);
        break;
    case '"':
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') {
                error("Unterminated string literal");
                break;
            }
            advance();
        }
        if (isAtEnd()) {
            error("Unterminated string literal");
        } else {
            advance();
            emitToken(TokenType::STRING_LITERAL);
        }
        break;
    default:
        if (isdigit(c)) {
            while (isdigit(peek()))
                advance();

            if (peek() == '.') {
                advance();

                while (isdigit(peek()) || peek() == '.') {
                    if (peek() == '.') {
                        error("Float literals must only contain one decimal point");
                    }

                    advance();
                }
                emitToken(TokenType::FLOAT_LITERAL);
            } else {
                emitToken(TokenType::INT_LITERAL);
            }
            break;
        }
        if (isalpha(c) || c == '_') {
            std::string identifier = "";
            identifier += c;
            while (isalnum(peek()) || peek() == '_') {
                identifier += peek();
                advance();
            }
            if (identifier == "return") {
                emitToken(TokenType::RETURN);
            } else if (identifier == "if") {
                emitToken(TokenType::IF);
            } else if (identifier == "else") {
                emitToken(TokenType::ELSE);
            } else if (identifier == "while") {
                emitToken(TokenType::WHILE);
            } else if (identifier == "for") {
                emitToken(TokenType::FOR);
            } else {
                emitToken(TokenType::IDENTIFIER);
            }
            break;
        }
        error(fmt::format("Invalid character '{}'", c));
        break;
    }
}

void Lexer::emitToken(TokenType type) {
    tokens.emplace_back(type, begin_location, end_location, getLexeme());
}

void Lexer::advance() {
    // Update location information
    if (peek() == '\n') {
        ++end_location.line;
        end_location.col = 1;
    } else {
        ++end_location.col;
    }

    if (!isAtEnd())
        ++end;
}

char Lexer::peek() {
    if (!isAtEnd())
        return *end;
    else
        return '\0';
}

void Lexer::error(const std::string &message) {
    errorFlag = true;
    llvm::WithColor::error(llvm::errs(), "lexer") << fmt::format(
        "{}:{}: {}\n", end_location.line, end_location.col, message);
}

std::string Lexer::getLexeme() const { return std::string{begin, end}; }
