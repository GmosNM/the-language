// astGen.hpp
#ifndef AST_GEN_HPP_
#define AST_GEN_HPP_

#include "ast.hpp"
#include <iostream>
#include <stdexcept>

class ASTGen {
public:
    ASTGen() : currentNode(nullptr) {}

    void addNode(Instruction *node) { nodes.push_back(node); }

    void printAST() {
        std::cout << "AST:" << std::endl;
        for (auto node : nodes) {
            switch (node->type) {
            case NodeType::VARIABLE_DECLARATION: {
                std::cout << "VariableDeclaration" << std::endl;
                VariableDeclaration *v =
                    dynamic_cast<VariableDeclaration *>(node);
                std::cout << "    Name: " << v->name << std::endl;
                std::cout << "    Type: " << dataTypeToString(v->variable_type)
                          << std::endl;
                if (v->initialization_value) {
                    std::cout << "    Initialization value: "
                              << v->initialization_value->literal_value
                              << std::endl;
                }
                break;
            }
            case NodeType::VARIABLE_REFERENCE:
                std::cout << "VariableReference" << std::endl;
                break;
            case NodeType::FUNCTION_DECLARATION: {
                FunctionDeclaration *f =
                    dynamic_cast<FunctionDeclaration *>(node);
                HandleFunctionDeclaration(f);
                break;
            }
            case NodeType::FUNCTION_CALL:
                std::cout << "FunctionCall" << std::endl;
                break;
            case NodeType::FUNCTION_BODY: {
                break;
            }
            default:
                break;
            }
        }
    }

    void HandleFunctionDeclaration(FunctionDeclaration *f) {
        std::cout << "FunctionDeclaration" << std::endl;
        std::cout << "    Name: " << f->name << std::endl;
        std::cout << "    Return type: " << dataTypeToString(f->return_type)
                  << std::endl;

        for (auto param : f->parameters) {
            std::cout << "\tParameter: " << param->name << std::endl;
            std::cout << "\t\tType: " << dataTypeToString(param->variable_type)
                      << std::endl;
        }

        processInstructions(f->body->getInstructions());
    }

    void processInstructions(const std::vector<Instruction *> &instructions) {
        for (auto instruction : instructions) {
            std::cout << "\tInstruction: "
                      << nodeTypeToString(instruction->type) << std::endl;

            switch (instruction->type) {
            case NodeType::RETURN_STATEMENT: {
                ReturnStatement *rs =
                    dynamic_cast<ReturnStatement *>(instruction);
                handelReturnStatement(rs);
                break;
            }

            case NodeType::VARIABLE_DECLARATION: {
                VariableDeclaration *v =
                    dynamic_cast<VariableDeclaration *>(instruction);
                std::cout << "\t\tVariable Declaration: " << v->name
                          << std::endl;
                std::cout << "\t\tType: " << dataTypeToString(v->variable_type)
                          << std::endl;
                if (!v->initialization_value->literal_value.empty()) {
                    std::cout << "\t\tInitialization value: "
                              << v->initialization_value->literal_value
                              << std::endl;
                }
                if (!v->initialization_value->function_name.empty()) {
                    std::cout << "\t\tFunction Name: "
                              << v->initialization_value->function_name
                              << std::endl;
                }
                for (const auto &arg : v->initialization_value->arguments) {
                    std::cout << "\t\t\tArgument:\n\t\t\tname: \""
                              << arg->variable_name
                              << "\"\n\t\t\tValue: " << arg->literal_value
                              << std::endl;
                }
                break;
            }
            case NodeType::VARIABLE_ASSIGNMENT: {
                VariableAssignment *v =
                    dynamic_cast<VariableAssignment *>(instruction);
                std::cout << "\t\tVariable Assignment: " << v->name
                          << std::endl;
                if (v->new_value) {
                    std::cout
                        << "\t\tNew value: " << v->new_value->literal_value
                        << std::endl;
                }
                break;
            }

            case NodeType::IF: {
                IfStatement *i = dynamic_cast<IfStatement *>(instruction);
                std::cout << "\t\tIf Statement:" << std::endl;
                std::cout << "\t\t\tCondition: "
                          << i->condition->left_operand->literal_value
                          << std::endl;
                std::cout << "\t\t\tOperator: "
                          << operationToString(i->condition->operation)
                          << std::endl;
                std::cout << "\t\t\tRight operand: "
                          << i->condition->right_operand->literal_value
                          << std::endl;
                processInstructions(i->ifBody->getInstructions());
                break;
            }

            case NodeType::ELSE: {
                ElseStatement *i = dynamic_cast<ElseStatement *>(instruction);
                processInstructions(i->elseBody->getInstructions());
                break;
            }

            case NodeType::PRINT_NODE: {
                PrintNode *printNode = dynamic_cast<PrintNode *>(instruction);
                std::cout << "\tPrint Statement: " << printNode->function_name
                          << " (Built-in)" << std::endl;

                if (!printNode->arguments.empty()) {
                    std::cout << "\t\tArguments:" << std::endl;
                    for (const auto &arg : printNode->arguments) {
                        std::cout << "\t\t\t" << arg << std::endl;
                    }
                } else {
                    std::cout << "\t\tNo arguments" << std::endl;
                }
                break;
            }

            case NodeType::EXPRESSION: {
                Expression *e = dynamic_cast<Expression *>(instruction);
                switch (e->type) {
                case Expression::Type::LITERAL:
                    std::cout << "\t\tLiteral: " << e->literal_value
                              << std::endl;
                    break;
                case Expression::Type::VARIABLE:
                    std::cout << "\t\tVariable: " << e->variable_name
                              << std::endl;
                    break;
                case Expression::Type::BINARY_OPERATION:
                    std::cout << "\t\t\tLeft operand: "
                              << e->left_operand->literal_value << std::endl;
                    std::cout
                        << "\t\t\tOperator: " << operationToString(e->operation)
                        << std::endl;
                    std::cout << "\t\t\tRight operand: "
                              << e->right_operand->literal_value << std::endl;
                    break;
                case Expression::Type::FUNCTION_CALL:
                    std::cout << "\t\tFunction Call: " << e->function_name
                              << std::endl;
                    for (const auto &arg : e->arguments) {
                        std::cout << "\t\t\t" << arg->literal_value
                                  << std::endl;
                    }
                    break;
                case Expression::Type::PRINT:
                    std::cout << "\t\tPrint: " << e->function_name << std::endl;
                    break;
                default:
                    std::cout << "Unknown" << std::endl;
                    break;
                }
            }

            default:
                std::cout << "Unknown" << std::endl;
                break;
            }
        }
    }

    void handelReturnStatement(ReturnStatement *rs) {
        switch (rs->returned_value->type) {
        case Expression::Type::LITERAL:
            std::cout << "\t\tReturned value (LITERAL): "
                      << rs->returned_value->literal_value << std::endl;
            break;

        case Expression::Type::VARIABLE:
            std::cout << "\t\tReturned value (VARIABLE): "
                      << rs->returned_value->variable_name << std::endl;
            break;

        case Expression::Type::BINARY_OPERATION:
            std::cout << "\t\t\tLeft operand: "
                      << rs->returned_value->left_operand->literal_value
                      << std::endl;
            std::cout << "\t\t\tOperator: "
                      << operationToString(rs->returned_value->operation)
                      << std::endl;
            std::cout << "\t\t\tRight operand: "
                      << rs->returned_value->right_operand->literal_value
                      << std::endl;
            break;

        case Expression::Type::FUNCTION_CALL:
            std::cout << "\t\tReturned value (FUNCTION_CALL): "
                      << rs->returned_value->function_name << std::endl;
            for (const auto &arg : rs->returned_value->arguments) {
                std::cout << "\t\t\tArgument: " << arg->literal_value
                          << std::endl;
            }
            break;

        case Expression::Type::VARIABLE_REFERENCE:
            if (!rs->returned_value->variable_reference->initialization_value
                     ->function_name.empty()) {
                std::cout << "\t\tReturned value (VARIABLE_REFERENCE => "
                             "FunctionCall): "
                          << rs->returned_value->variable_reference->name
                          << std::endl;
                std::cout << "\t\t\tType: "
                          << dataTypeToString(
                                 rs->returned_value->variable_reference
                                     ->initialization_value->variable_type)
                          << std::endl;
                std::cout << "\t\t\tValue: "
                          << rs->returned_value->variable_reference
                                 ->initialization_value->function_name
                          << std::endl;
                for (const auto &arg : rs->returned_value->variable_reference
                                           ->initialization_value->arguments) {
                    std::cout << "\t\t\t\tArgument: \n\t\t\t\tname: \""
                              << arg->variable_name
                              << "\" \n\t\t\t\tValue: " << arg->literal_value
                              << std::endl;
                }
                std::cout << "\t\tReturned value (VARIABLE_REFERENCE): "
                          << rs->returned_value->variable_reference->name
                          << std::endl;
                break;
            } else {
                std::cout << "\t\tReturned value (VARIABLE_REFERENCE): "
                          << rs->returned_value->variable_name << std::endl;
                std::cout << "\t\t\tType: "
                          << dataTypeToString(
                                 rs->returned_value->variable_type.category)
                          << std::endl;
                std::cout << "\t\t\tValue: "
                          << rs->returned_value->literal_value << std::endl;
                break;
            }

        case Expression::Type::VARIABLE_ASSIGNMENT:
            std::cout << "\t\tReturned value (VARIABLE_ASSIGNMENT): "
                      << rs->returned_value->variable_name << std::endl;
            std::cout << "\t\t\tType: "
                      << dataTypeToString(rs->returned_value->variable_type)
                      << std::endl;
            if (rs->returned_value->function_name.empty()) {
                std::cout << "\t\t\tValue: "
                          << rs->returned_value->literal_value << std::endl;
            } else {
                std::cout << "\t\t\tFunction Name: "
                          << rs->returned_value->function_name << std::endl;
            }
            break;

        default:
            break;
        }
    }

    std::vector<Instruction *> nodes;
    Instruction *currentNode;
};

#endif // AST_GEN_HPP_
