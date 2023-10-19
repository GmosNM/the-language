#include "lexer.hpp"

void Lexer::read() {
    file_stream.open(file_name);
    if (!file_stream.is_open()) {
        std::cerr << "Cannot open file " << file_name << std::endl;
    }
    std::string line;
    while (std::getline(file_stream, line)) {
        file_content += line + "\n";
    }
    file_content[file_content.size() - 1] = '\0';
}

void Lexer::lex() {
    int i = 0;
    int line = 1;
    int col = 1;

    while (i < file_content.size()) {
        while (i < file_content.size() &&
               (isDelimiter(file_content[i]) || isSpace(file_content[i]))) {
            if (file_content[i] == '\n') {
                line++;
                col = 1;
            } else if (file_content[i] != ' ') {
                col++;
            }
            i++;
        }

        if (file_content[i] == '#') {
            while (i < file_content.size() && file_content[i] != '\n') {
                col++;
                i++;
            }
        }

        int word_end = i;

        if (file_content[word_end] == '"') {
            word_end++;
            while (word_end < file_content.size() &&
                   file_content[word_end] != '"') {
                word_end++;
            }
            if (word_end < file_content.size() &&
                file_content[word_end] == '"') {
                word_end++;
            }
        } else {
            while (word_end < file_content.size() &&
                   !isDelimiter(file_content[word_end]) &&
                   !isSpace(file_content[word_end]) &&
                   !isBreaker(file_content[word_end])) {
                word_end++;
            }
        }

        if (i < word_end) {
            std::string token = file_content.substr(i, word_end - i);
            words.push_back(token);
        }

        if (isBreaker(file_content[word_end])) {
            words.push_back(std::string(1, file_content[word_end]));
        }

        i = word_end;
        i++;
        col++;
    }
}

bool Lexer::hasNextToken() { return token_index + 1 < tokens.size(); }

Token Lexer::getNextToken() { return *tokens[token_index++]; }

Token Lexer::peekNextToken() { return *tokens[token_index]; }

void Lexer::nextToken() {
    if (hasNextToken()) {
        token_index++;
    }
}
void Lexer::tokenalize() {
    while (word_index < words.size()) {
        try {
            std::unique_ptr<Token> token = std::make_unique<Token>();
            std::string word = words[word_index];

            if (word == "int") {
                token->type = INT;
                token->value = word;
            } else if (word == "let") {
                token->type = LET;
                token->value = word;
            } else if (word == "float") {
                token->type = FLOAT;
                token->value = word;
            } else if (word == "bool") {
                token->type = BOOL;
                token->value = word;
            } else if (word == "char") {
                token->type = CHAR;
                token->value = word;
            } else if (word == "return") {
                token->type = RETURN;
                token->value = word;
            } else if (word == "test") {
                token->type = TEST;
                token->value = word;
            } else if (word == "println") {
                token->type = PRINTLN_KW;
                token->value = word;
            } else if (word == "true") {
                token->type = TRUE;
                token->value = word;
            } else if (word == "false") {
                token->type = FALSE;
                token->value = word;
            } else if (word == "fn") {
                token->type = FUNCTION;
                token->value = word;
            } else if (word == "if") {
                token->type = IF;
                token->value = word;
            } else if (word == "else") {
                token->type = ELSE;
                token->value = word;
            } else if (isNumber(word[0])) {
                bool is_float = false;
                for (int j = 0; j < word.size(); j++) {
                    if (word[j] == '.') {
                        is_float = true;
                    }
                }
                if (is_float) {
                    token->type = FLOAT_LITERAL;
                    token->value = word;
                } else {
                    token->type = NUMBER;
                }
                token->value = word;
            } else if (word[0] == ';') {
                token->type = SEMICOLON;
                token->value = word;
            } else if (word[0] == '{') {
                token->type = LBRACE;
                token->value = "{";
            } else if (word[0] == '}') {
                token->type = RBRACE;
                token->value = "}";
            } else if (word[0] == '(') {
                token->type = LPAREN;
                token->value = word;
            } else if (word[0] == ')') {
                token->type = RPAREN;
                token->value = word;
            } else if (word[0] == ':') {
                token->type = COLON;
                token->value = word;
            } else if (word == "string") {
                token->type = STRING;
                token->value = word;
            } else if (word[0] == ',') {
                token->type = COMMA;
                token->value = word;
            } else if (word[0] == '"') {
                std::string content;
                for (int j = 0; j < word.size(); j++) {
                    if (word[j] != '"') {
                        content += word[j];
                    }
                }
                token->type = STRING_LITERAL;
                token->value = content;
            } else if (word[0] == '+') {
                token->type = PLUS;
                token->value = word;
            } else if (word[0] == '-') {
                token->type = MINUS;
                token->value = word;
            } else if (word[0] == '*') {
                token->type = STAR;
                token->value = word;
            } else if (word[0] == '/') {
                token->type = SLASH;
                token->value = word;
            } else if (word[0] == '>') {
                token->type = GREATER;
                token->value = word;
            } else if (word[0] == '<') {
                token->type = LESS;
                token->value = word;
            } else if (word[0] == '=') {
                token->type = EQUAL;
                token->value = word;
            } else if (word[0] == '\0') {
                token->type = EoF;
                token->value = word;
            } else {
                token->type = IDENTIFIER;
                token->value = word;
            }

            tokens.push_back(std::move(token));
            word_index++;
            token_index++;
        } catch (std::exception &e) {
            std::cerr << "Error tokenizing word '" << words[word_index]
                      << "': " << e.what() << std::endl;
        }
    }
}
