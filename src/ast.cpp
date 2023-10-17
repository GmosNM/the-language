#include "ast.hpp"

std::string nodeTypeToString(NodeType type) {
    switch (type) {
    case NodeType::VARIABLE_DECLARATION:
        return "VariableDeclaration";
    case NodeType::VARIABLE_REFERENCE:
        return "VariableReference";
    case NodeType::FUNCTION_DECLARATION:
        return "FunctionDeclaration";
    case NodeType::FUNCTION_CALL:
        return "FunctionCall";
    case NodeType::FUNCTION_BODY:
        return "FunctionBody";
    case NodeType::RETURN_STATEMENT:
        return "Return";
    case NodeType::EXPRESSION:
        return "Expression";
    case NodeType::VARIABLE_ASSIGNMENT:
        return "VariableAssignment";
    case NodeType::IF:
        return "IfStatement";
    case NodeType::ELSE:
        return "ElseStatement";
    default:
        return "Unknown";
    }
}

std::string dataTypeToString(DataType type) {
    switch (type.category) {
    case DataType::Category::INT:
        return "INT";
    case DataType::Category::FLOAT:
        return "FLOAT";
    case DataType::Category::BOOL:
        return "BOOL";
    case DataType::Category::CHAR:
        return "CHAR";
    case DataType::Category::STRING:
        return "STRING";
    default:
        return "Unknown";
    }
}

std::string dataTypeToCType(DataType type) {
    switch (type.category) {
    case DataType::Category::INT:
        return "int";
    case DataType::Category::FLOAT:
        return "float";
    case DataType::Category::BOOL:
        return "bool";
    case DataType::Category::CHAR:
        return "char";
    case DataType::Category::STRING:
        return "char*";
    default:
        return "Unknown";
    }
}

std::string operationToString(Operation op) {
    switch (op) {
    case Operation::ADD:
        return "+";
    case Operation::SUBTRACT:
        return "-";
    case Operation::MULTIPLY:
        return "*";
    case Operation::DIVIDE:
        return "/";
    case Operation::EQUAL:
        return "==";
    default:
        return "Unknown";
    }
}
