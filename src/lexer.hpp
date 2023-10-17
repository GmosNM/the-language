#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

typedef enum TokenType {
    UNKNOWN,
    EoF,
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    STRING_LITERAL,
    FLOAT_LITERAL,
    OPERATOR,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    COMMA,
    SEMICOLON,
    COLON,
    EQUAL,
    NOT_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    // Types
    INT,
    FLOAT,
    BOOL,
    CHAR,
    VOID,
    STRING,
    // Keywords
    RETURN,
    FUNCTION,
    LET,
    TRUE,
    FALSE,
    TEST,
    IF,
    ELSE,
} TokenType;

typedef struct Token {
    TokenType type;
    std::string value;
    int col;
    int line;
} Token;

class Lexer {
public:
    Token currentToken;
    Lexer(std::string file_name) { this->file_name = file_name; }

    void read();
    void lex();
    void tokenalize();

    auto print_content() {
        for (auto &w : words) {
            std::cout << w << std::endl;
        }
    }

    auto token_to_string(TokenType type) {
        switch (type) {
        case UNKNOWN:
            return "UNKNOWN";
        case EoF:
            return "End of File";
        case KEYWORD:
            return "KEYWORD";
        case IDENTIFIER:
            return "IDENTIFIER";
        case STRING:
            return "STRING";
        case NUMBER:
            return "NUMBER";
        case OPERATOR:
            return "OPERATOR";
        case LPAREN:
            return "LPAREN";
        case RPAREN:
            return "RPAREN";
        case LBRACE:
            return "LBRACE";
        case RBRACE:
            return "RBRACE";
        case LBRACKET:
            return "LBRACKET";
        case RBRACKET:
            return "RBRACKET";
        case COMMA:
            return "COMMA";
        case SEMICOLON:
            return "SEMICOLON";
        case COLON:
            return "COLON";
        case EQUAL:
            return "EQUAL";
        case NOT_EQUAL:
            return "NOT_EQUAL";
        case GREATER:
            return "GREATER";
        case GREATER_EQUAL:
            return "GREATER_EQUAL";
        case LESS:
            return "LESS";
        case LESS_EQUAL:
            return "LESS_EQUAL";
        case PLUS:
            return "PLUS";
        case MINUS:
            return "MINUS";
        case STAR:
            return "STAR";
        case SLASH:
            return "SLASH";
        case INT:
            return "INT";
        case FLOAT:
            return "FLOAT";
        case BOOL:
            return "BOOL";
        case CHAR:
            return "CHAR";
        case VOID:
            return "VOID";
        case RETURN:
            return "RETURN";
        case FUNCTION:
            return "FUNCTION";
        case LET:
            return "LET";
        case STRING_LITERAL:
            return "STRING_LITERAL";
        case FLOAT_LITERAL:
            return "FLOAT_LITERAL";
        case TRUE:
            return "TRUE";
        case FALSE:
            return "FALSE";
        case TEST:
            return "TEST";
        case IF:
            return "IF";
        case ELSE:
            return "ELSE";
        }
        return "UNKNOWN";
    }

    auto print_tokens() {
        for (auto &t : tokens) {
            std::cout << token_to_string(t->type) << " " << t->value
                      << std::endl;
        }
    }

    ~Lexer() {
        if (file_stream.is_open()) {
            file_stream.close();
        }
    }

    bool hasNextToken();
    Token getNextToken();
    Token peekNextToken();
    void nextToken();

    Token getCurrentToken() {
        if (token_index < tokens.size()) {
            return *tokens[token_index];
        } else {
            Token eofToken;
            eofToken.type = EoF;
            eofToken.value = "End of File";
            return eofToken;
        }
    }

    std::vector<std::unique_ptr<Token>> tokens;

protected:
    std::string file_name;
    std::string file_content;
    std::vector<std::string> words;
    uint32_t index = 0;
    uint32_t word_index = 0;
    uint32_t token_index = 0;
    std::vector<std::string> breakers = {"==", "&&", "->", "||", ")", "(", "{",
                                         "}",  "[",  "]",  ",",  ";", ":", ","};
    std::vector<char> delimiters = {' ', '\t', '\r', '\n'};
    std::ifstream file_stream;

    bool isBreaker(char c) {
        for (const std::string &breaker : breakers) {
            if (breaker.find(c) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    bool isDelimiter(char c) {
        return std::find(delimiters.begin(), delimiters.end(), c) !=
               delimiters.end();
    }

    bool isNumber(char c) { return c >= '0' && c <= '9'; }
};
