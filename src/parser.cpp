#include "parser.hpp"

using Type = DataType::Category;

void Parser::expect(TokenType type) {
    if (type == getCurrentToken().type) {
        return;
    }
    std::cerr << "Expected " << lexer.token_to_string(type) << " but got "
              << lexer.token_to_string(getCurrentToken().type) << std::endl;
}

void Parser::consume(TokenType type) {
    expect(type);
    index++;
}

Token Parser::getCurrentToken() { return *lexer.tokens[index]; }

Token Parser::getNextToken() {
    if (hasNextToken()) {
        return *lexer.tokens[index + 1];
    }
    throw std::runtime_error("There is no next token");
}

Token Parser::getPreviousToken() { return *lexer.tokens[index - 1]; }
bool Parser::hasNextToken() { return index + 1 < lexer.tokens.size(); }
bool Parser::match(TokenType type) {
    if (getCurrentToken().type == type) {
        return true;
    }
    return false;
}

bool Parser::isOperator(TokenType type) {
    switch (type) {
    case PLUS:
    case MINUS:
    case STAR:
    case SLASH:
    case EQUAL:
        return true;
    default:
        return false;
    }
}

Operation Parser::getOperationType(TokenType type) {
    switch (type) {
    case PLUS:
        return Operation::ADD;
    case MINUS:
        return Operation::SUBTRACT;
    case STAR:
        return Operation::MULTIPLY;
    case SLASH:
        return Operation::DIVIDE;
    case EQUAL:
        return Operation::EQUAL;
    default:
        throw std::runtime_error("Invalid operation type");
    }
}

void Parser::parse() {
    lexer.read();
    lexer.lex();
    lexer.tokenalize();
    lexer.print_tokens();
    index = 0;

    while (index < lexer.tokens.size()) {
        if (getCurrentToken().type == EoF) {
            break;
        }
        switch (getCurrentToken().type) {
        case FUNCTION:
            parseFunction();
            break;
        case LET: {
            auto a = parseVariableDeclaration();
            ast.addNode(a);
            break;
        }
        case IDENTIFIER: {
            auto a = parseVariableReference();
            std::cout << a->name << std::endl;
            ast.addNode(a);
            break;
        }
        default:
            break;
        }
    }

    ast.printAST();
    print_cuurent_scope();
}

void Parser::parseFunction() {
    expect(FUNCTION);
    consume(FUNCTION);

    std::string name = getCurrentToken().value;
    std::vector<VariableDeclaration *> parameters;
    expect(IDENTIFIER);
    consume(IDENTIFIER);

    expect(LPAREN);
    consume(LPAREN);

    enterScope();

    while (getCurrentToken().type != RPAREN) {
        std::string paramName = getCurrentToken().value;
        expect(IDENTIFIER);
        consume(IDENTIFIER);
        expect(COLON);
        consume(COLON);
        DataType paramType = parseDataType();

        Expression *param = new Expression(paramName, paramType);
        VariableDeclaration *var =
            new VariableDeclaration(paramName, paramType, param);
        globalSymbolTable->parentScope->AddVariable(paramName, var);
        parameters.emplace_back(var);
        if (getCurrentToken().type != RPAREN) {
            expect(COMMA);
            consume(COMMA);
        }
    }

    expect(RPAREN);
    consume(RPAREN);

    expect(COLON);
    consume(COLON);

    DataType returnType = parseDataType();

    expect(LBRACE);
    consume(LBRACE);

    FunctionBody *body = new FunctionBody();

    body = parseBody(body);

    expect(RBRACE);
    consume(RBRACE);

    FunctionDeclaration *func =
        new FunctionDeclaration(name, parameters, returnType, body);
    ast.addNode(func);
    globalSymbolTable->AddFunction(name, func);
    exitScope();
}

FunctionBody *Parser::parseBody(FunctionBody *body) {
    while (getCurrentToken().type != RBRACE) {
        if (getCurrentToken().type == LET) {
            auto var = parseVariableDeclaration();
            body->addInstruction(var);
            globalSymbolTable->parentScope->AddVariable(var->name, var);
        } else if (getCurrentToken().type == RETURN) {
            // TODO: Handle return type
            parseReturnStatement(body, DataType::Category::INT);
        } else if (getCurrentToken().type == IF) {
            auto stats = parseIfStatement();
            body->addInstruction(stats);
        } else if (getCurrentToken().type == ELSE) {
            consume(ELSE);
            FunctionBody *elseBody = new FunctionBody();
            expect(LBRACE);
            consume(LBRACE);
            elseBody = parseBody(elseBody);
            expect(RBRACE);
            consume(RBRACE);
            ElseStatement *elseStatement = new ElseStatement(elseBody, body);
            body->addInstruction(elseStatement);
        } else if (getCurrentToken().type == PRINTLN_KW) {
            auto print = parsePrintStatement();
            body->addInstruction(print);
        } else if (getCurrentToken().type == IDENTIFIER) {
            if (getNextToken().type == EQUAL) {
                auto var = parseVariableAssignment();
                body->addInstruction(var);
            } else {
                auto var = parseVariableReference();
                body->addInstruction(var);
            }
        } else {
            std::cerr << "Unexpected token: "
                      << lexer.token_to_string(getCurrentToken().type)
                      << std::endl;
            break;
        }
    }
    return body;
}

void Parser::parseReturnStatement(FunctionBody *body, DataType returnType) {
    expect(RETURN);
    consume(RETURN);

    Expression *expression = parseExpression();

    if (expression) {
        body->addReturnStatement(expression);
        body->setReturnType(returnType);
        consume(SEMICOLON);
    }
}

VariableDeclaration *Parser::parseVariableDeclaration() {
    expect(LET);
    consume(LET);
    std::string name = getCurrentToken().value;
    consume(IDENTIFIER);
    expect(COLON);
    consume(COLON);
    DataType type = parseDataType();
    Expression *initialization_value = nullptr;

    if (match(EQUAL)) {
        consume(EQUAL);
        if (match(IDENTIFIER)) {
            std::string functionName = getCurrentToken().value;
            consume(IDENTIFIER);
            std::vector<Expression *> arguments;
            expect(LPAREN);
            consume(LPAREN);

            while (getCurrentToken().type != RPAREN) {
                Expression *argument = parseExpression();
                arguments.push_back(argument);
                if (match(COMMA)) {
                    consume(COMMA);
                } else {
                    break;
                }
            }

            expect(RPAREN);
            consume(RPAREN);

            initialization_value =
                new FunctionCall(functionName, arguments, type);
        } else {
            initialization_value = parseExpression();
        }
    }

    VariableDeclaration *variableDeclaration =
        new VariableDeclaration(name, type, initialization_value);
    expect(SEMICOLON);
    consume(SEMICOLON);

    if (scopeStack.empty()) {
        globalSymbolTable->AddVariable(name, variableDeclaration);
    }

    return variableDeclaration;
}

VariableReference *Parser::parseVariableReference() {
    std::string name = getCurrentToken().value;
    if (globalSymbolTable->parentScope->hasVariable(name)) {
        auto var = globalSymbolTable->parentScope->GetVariable(name);
        return new VariableReference(
            name, var->initialization_value,
            globalSymbolTable->parentScope->GetVariable(name)->variable_type);
    }
    return nullptr;
}

Expression *Parser::parseCondition() {
    if (match(LPAREN)) {
        consume(LPAREN);

        expect(IDENTIFIER);
        std::string identifier = getCurrentToken().value;
        VariableReference *ref = parseVariableReference();
        consume(IDENTIFIER);

        Expression *expression =
            new Expression(identifier, ref->variable_type, ref->name);

        consume(EQUAL);

        Expression *right = parseTerm();

        expect(RPAREN);
        consume(RPAREN);

        return new Expression(Operation::EQUAL, expression, right);
    }

    throw std::runtime_error("Invalid condition");
    return nullptr;
}

Expression *Parser::parseExpression() {
    Expression *expression = parseTerm();

    if (match(TRUE)) {
        consume(TRUE);
        expression = new Expression(true);
        return expression;
    } else if (match(FALSE)) {
        consume(FALSE);
        expression = new Expression(false);
        return expression;
    }

    while (match(LPAREN)) {
        consume(LPAREN);
        std::string funcName = expression->variable_name;
        std::vector<Expression *> args;

        while (!match(RPAREN)) {
            Expression *arg = parseExpression();
            args.push_back(arg);
            if (match(COMMA)) {
                consume(COMMA);
            }
        }

        expect(RPAREN);
        consume(RPAREN);

        DataType returnType = determineFunctionReturnType(funcName, args);
        Expression *funcCall = new Expression(funcName, args, returnType);
        expression = funcCall;
    }

    while (isOperator(getCurrentToken().type)) {
        Token op = getCurrentToken();
        consume(op.type);
        Expression *right = parseTerm();
        expression =
            new Expression(getOperationType(op.type), expression, right);
    }

    return expression;
}

Expression *Parser::parseTerm() {
    Expression *left = parseFactor();

    while (getCurrentToken().type == STAR || getCurrentToken().type == SLASH) {
        TokenType op = getCurrentToken().type;
        consume(op);

        Expression *right = parseFactor();

        if (op == STAR) {
            left = new Expression(Operation::MULTIPLY, left, right);
        } else if (op == SLASH) {
            left = new Expression(Operation::DIVIDE, left, right);
        }
    }

    return left;
}

Expression *Parser::parseFactor() {
    Expression *primary = nullptr;

    if (getCurrentToken().type == NUMBER) {
        primary =
            new Expression(getCurrentToken().value, DataType::Category::INT);
        consume(NUMBER);
    } else if (getCurrentToken().type == FLOAT_LITERAL) {
        primary =
            new Expression(getCurrentToken().value, DataType::Category::FLOAT);
        consume(FLOAT_LITERAL);
    } else if (getCurrentToken().type == BOOL) {
        primary =
            new Expression(getCurrentToken().value, DataType::Category::BOOL);
        consume(BOOL);
        if (getCurrentToken().value == "true") {
            consume(TRUE);
        } else if (getCurrentToken().value == "false") {
            consume(FALSE);
        }
    } else if (getCurrentToken().type == CHAR) {
        primary =
            new Expression(getCurrentToken().value, DataType::Category::CHAR);
        consume(CHAR);
    } else if (getCurrentToken().type == STRING_LITERAL) {
        primary = new Expression("\"" + getCurrentToken().value + "\"",
                                 DataType::Category::STRING);
        consume(STRING_LITERAL);
    } else if (getCurrentToken().type == IDENTIFIER) {
        std::string variableName = getCurrentToken().value;
        consume(IDENTIFIER);

        if (globalSymbolTable->parentScope->hasVariable(variableName)) {
            DataType variableType =
                globalSymbolTable->parentScope->GetVariable(variableName)
                    ->variable_type;
            std::string variableValue =
                globalSymbolTable->GetVariable(variableName)
                    ->initialization_value->literal_value;
            primary = new Expression(variableName, parseVariableReference(),
                                     variableType, variableValue);
            primary->variable_reference =
                globalSymbolTable->parentScope->GetVariableRef(variableName);
        } else {
            std::cerr << "Variable '" << variableName << "' is undefined."
                      << std::endl;
        }
    } else if (getCurrentToken().type == LPAREN) {
        consume(LPAREN);
        primary = parseExpression();
        expect(RPAREN);
        consume(RPAREN);
    } else {
    }

    return primary;
}

DataType Parser::parseDataType() {
    DataType type = DataType::Category::UNKNOWN;
    if (getCurrentToken().type == INT) {
        type = DataType::Category::INT;
        consume(INT);
    } else if (getCurrentToken().type == FLOAT) {
        type = DataType::Category::FLOAT;
        consume(FLOAT);
    } else if (getCurrentToken().type == BOOL) {
        type = DataType::Category::BOOL;
        consume(BOOL);
    } else if (getCurrentToken().type == CHAR) {
        type = DataType::Category::CHAR;
        consume(CHAR);
    } else if (getCurrentToken().type == STRING) {
        type = DataType::Category::STRING;
        consume(STRING);
    } else {
        throw std::runtime_error("Invalid data type");
    }
    return type;
}

DataType
Parser::determineFunctionReturnType(const std::string &functionName,
                                    const std::vector<Expression *> &args) {
    if (globalSymbolTable->parentScope->hasFunction(functionName)) {
        DataType functionType =
            globalSymbolTable->parentScope->GetFunction(functionName)
                ->return_type;

        if (functionType.category != DataType::Category::UNKNOWN) {
            return functionType.category;
        } else {
            std::cerr << "Function '" << functionName
                      << "' has an undefined return type." << std::endl;
        }
    } else {
        std::cerr << "Function '" << functionName << "' is undefined."
                  << std::endl;
    }

    return DataType(DataType::Category::UNKNOWN);
}

VariableAssignment *Parser::parseVariableAssignment() {
    std::string name = getCurrentToken().value;
    consume(IDENTIFIER);
    expect(EQUAL);
    consume(EQUAL);

    Expression *assignmentValue = parseExpression();

    expect(SEMICOLON);
    consume(SEMICOLON);

    auto *var = globalSymbolTable->parentScope->GetVariable(name);

    if (var) {
        Expression *oldValue = var->initialization_value;
        globalSymbolTable->parentScope->setNewVariableValue(name,
                                                            assignmentValue);
        return new VariableAssignment(var, oldValue, assignmentValue);
    } else {
        std::cerr << "Variable not found: " << name << std::endl;
        return nullptr;
    }
}

IfStatement *Parser::parseIfStatement() {
    expect(IF);
    consume(IF);

    Expression *condition = parseCondition();
    FunctionBody *ifBody = new FunctionBody();
    FunctionBody *elseBody = new FunctionBody();

    expect(LBRACE);
    consume(LBRACE);

    ifBody = parseBody(ifBody);

    expect(RBRACE);
    consume(RBRACE);

    return new IfStatement(condition, ifBody, elseBody);
}

PrintNode *Parser::parsePrintStatement() {
    PrintNode *printNode = nullptr;
    expect(PRINTLN_KW);
    std::string functionName = getCurrentToken().value;
    consume(PRINTLN_KW);

    expect(LPAREN);
    consume(LPAREN);

    std::vector<std::string> stringArgs;
    std::vector<Expression *> expressionArgs;

    if (match(STRING_LITERAL)) {
        std::string argValue = getCurrentToken().value;
        consume(STRING_LITERAL);
        stringArgs.push_back(argValue);
    }

    if (match(COMMA)) {
        consume(COMMA);
    }

    while (!match(RPAREN)) {
        if (match(COMMA)) {
            consume(COMMA);
        }
        std::string argValue = getCurrentToken().value;
        if (globalSymbolTable->parentScope->hasVariable(argValue)) {
            VariableReference *var =
                globalSymbolTable->parentScope->GetVariableRef(argValue);
            expressionArgs.push_back(
                new Expression(var->name, var->variable_type,
                               var->initialization_value->literal_value));
        } else {
            Expression *arg = parseExpression();
            expressionArgs.push_back(arg);
        }

        consume(IDENTIFIER);
    }

    expect(RPAREN);
    consume(RPAREN);

    expect(SEMICOLON);
    consume(SEMICOLON);

    printNode = new PrintNode(functionName, stringArgs, expressionArgs);
    return printNode;
}
