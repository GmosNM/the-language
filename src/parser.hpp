#ifndef PARSER_HPP_
#define PARSER_HPP_

// clang-format off
#include "lexer.hpp"
#include <unordered_map>
#include "astGen.hpp"
// clang-format on

struct SymbolTable {
    std::unordered_map<std::string, VariableDeclaration *> variables;
    std::unordered_map<std::string, FunctionDeclaration *> functions;

    SymbolTable *parentScope;

    void AddVariable(const std::string &name, VariableDeclaration *variable) {
        variables[name] = variable;
    }

    void AddFunction(const std::string &name, FunctionDeclaration *function) {
        functions[name] = function;
    }

    VariableDeclaration *GetVariable(const std::string &name) {
        if (variables.count(name) > 0) {
            return variables[name];
        } else if (parentScope) {
            return parentScope->GetVariable(name);
        }
        return nullptr;
    }

    VariableReference *GetVariableRef(const std::string &name) {
        if (variables.count(name) > 0) {
            return new VariableReference(name,
                                         variables[name]->initialization_value,
                                         variables[name]->variable_type);
        } else if (parentScope) {
            return parentScope->GetVariableRef(name);
        }
        return nullptr;
    }

    FunctionDeclaration *GetFunction(const std::string &name) {
        if (functions.count(name) > 0) {
            return functions[name];
        } else if (parentScope) {
            return parentScope->GetFunction(name);
        }
        return nullptr;
    }

    bool hasFunction(const std::string &name) {
        for (auto &func : functions) {
            if (func.first == name) {
                return true;
            }
        }
        return false;
    }

    bool hasVariable(const std::string &name) {
        for (auto &var : variables) {
            if (var.first == name) {
                return true;
            }
        }
        return false;
    }

    void setNewVariableValue(const std::string &name,
                             const std::string &newValue) {
        if (hasVariable(name)) {
            VariableDeclaration *variable = GetVariable(name);
            variable->initialization_value->literal_value = newValue;
        }
    }

    SymbolTable(SymbolTable *parent = nullptr) : parentScope(parent) {}

    ~SymbolTable() {}
};

class Parser {

private:
    Lexer &lexer;
    size_t index;

    void expect(TokenType type);
    Token getCurrentToken();
    Token getNextToken();
    Token getPreviousToken();
    bool hasNextToken();
    void consume(TokenType type);
    bool match(TokenType type);

    bool isOperator(TokenType type);
    Operation getOperationType(TokenType type);

    DataType parseDataType();
    DataType determineFunctionReturnType(const std::string &functionName,
                                         const std::vector<Expression *> &args);
    // Parser
    void parseFunction();
    void parseReturnStatement(FunctionBody *body, DataType returnType);
    VariableDeclaration *parseVariableDeclaration();
    VariableReference *parseVariableReference();
    Expression *parseExpression();
    Expression *parseTerm();
    Expression *parseFactor();
    VariableAssignment *parseVariableAssignment();

    // SymbolTable

    void enterScope() {
        SymbolTable *newScope = new SymbolTable(globalSymbolTable);
        scopeStack.push_back(newScope);
        globalSymbolTable = newScope;
    }

    void exitScope() {
        if (!scopeStack.empty()) {
            SymbolTable *parentScope = scopeStack.back();
            scopeStack.pop_back();
            delete globalSymbolTable;
            globalSymbolTable = parentScope;
        } else {
            throw std::runtime_error("Attempted to exit the global scope");
        }
    }

    void print_cuurent_scope() {}

    std::vector<SymbolTable *> scopeStack;

public:
    SymbolTable *globalSymbolTable;
    Parser(Lexer &l) : lexer(l), index(0), globalSymbolTable(nullptr) {
        globalSymbolTable = new SymbolTable(nullptr);
    }
    Parser(const Parser &) = delete;

    ~Parser() {}

    void parse();

    void printAST() { ast.printAST(); }

    ASTGen ast;
};

#endif /* PARSER_HPP_ */
