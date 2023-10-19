#include "astGen.hpp"
#include "parser.hpp"

class IR {
public:
    IR(std::string inputFileName, std::string outputFileName, ASTGen &astGen,
       Parser &parser)
        : inputFileName(inputFileName), outputFileName(outputFileName),
          astGen(astGen), parser(parser), indentationLevel(0) {
        openInputFile();
        createOutputFile();
    }

    void closeFiles() {
        if (inputFile) {
            fclose(inputFile);
        }
        if (outputFile) {
            fclose(outputFile);
        }
    }

    void openInputFile() {
        inputFile = fopen(inputFileName.c_str(), "r");
        if (!inputFile) {
            throw std::runtime_error("Failed to open input file");
        }
    }

    void createOutputFile() {
        outputFile = fopen(outputFileName.c_str(), "w+");
        if (!outputFile) {
            throw std::runtime_error("Failed to create output file");
        }
    }

    void copyInputToFile() {
        if (inputFile && outputFile) {
            char buffer[1024];
            size_t bytesRead;

            while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) >
                   0) {
                fwrite(buffer, 1, bytesRead, outputFile);
            }
        }
    }

    void increaseIndentation() { indentationLevel += 1; }

    void decreaseIndentation() { indentationLevel -= 1; }

    void writeTabs() {
        for (int i = 0; i < indentationLevel; ++i) {
            writeToFile("\t");
        }
    }

    void writeIncludes() {
        writeToFile("#include <stdio.h>\n");
        writeToFile("#include <stdlib.h>\n");
        writeToFile("#include <string.h>\n");
        writeToFile("#include <stdarg.h>\n");
        writeToFile("#include <stdbool.h>\n");
        writeToFile("\n\n");

        const char *printlnfunction =
            "void println(const char *format, ...) {\n"
            "    va_list args;\n"
            "    va_start(args, format);\n"
            "\n"
            "    for (int i = 0; format[i] != '\\0'; i++) {\n"
            "        if (format[i] == '{' && format[i + 1] == '}') {\n"
            "            i += 1;\n"
            "            const char *arg = va_arg(args, const char *);\n"
            "            printf(\"%s\", arg);\n"
            "        } else {\n"
            "            putchar(format[i]);\n"
            "        }\n"
            "    }\n"
            "\n"
            "    va_end(args);\n"
            "    printf(\"\\n\");\n"
            "}\n";

        writeToFile(printlnfunction);

        writeToFile("\n\n");
    }

    void writeToFile(const std::string &str) {
        if (outputFile) {
            fwrite(str.c_str(), sizeof(char), str.size(), outputFile);
        } else {
            throw std::runtime_error("Output file is not open");
        }
    }

    void writeFunctionBody(const std::vector<Instruction *> &instructions) {
        for (auto instruction : instructions) {
            switch (instruction->type) {
            case NodeType::VARIABLE_DECLARATION: {
                VariableDeclaration *v =
                    dynamic_cast<VariableDeclaration *>(instruction);
                switch (v->initialization_value->type) {
                case Expression::Type::VARIABLE_REFERENCE:
                    writeToFile(dataTypeToCType(v->variable_type) + "    " +
                                v->name + " = ");
                    writeToFile(v->initialization_value->variable_name);
                    writeToFile(";\n");
                    break;
                case Expression::Type::FUNCTION_CALL:
                    writeToFile("    " + dataTypeToCType(v->variable_type) +
                                " " + v->name + " = ");
                    writeToFile(v->initialization_value->function_name);
                    writeToFile("(");
                    for (size_t i = 0;
                         i < v->initialization_value->arguments.size(); ++i) {
                        if (i > 0) {
                            writeToFile(", ");
                        }
                        writeToFile(v->initialization_value->arguments[i]
                                        ->variable_name);
                    }
                    writeToFile(")");
                    writeToFile(";\n");
                    break;
                default:
                    writeToFile("    " + dataTypeToCType(v->variable_type) +
                                " " + v->name + " = ");
                    writeToFile(v->initialization_value->literal_value);
                    writeToFile(";\n");
                    break;
                }
                break;
            }
            case NodeType::VARIABLE_ASSIGNMENT: {
                VariableAssignment *assign =
                    dynamic_cast<VariableAssignment *>(instruction);
                switch (assign->new_value->type) {
                case Expression::Type::VARIABLE_REFERENCE:
                    writeToFile("    " + assign->name + " = ");
                    writeToFile(assign->new_value->variable_name);
                    writeToFile(";\n");
                    break;
                default:
                    writeToFile("    " + assign->name + " = ");
                    writeToFile(assign->new_value->literal_value);
                };
                writeToFile(";\n");
                break;
            }
            case NodeType::FUNCTION_CALL: {
                FunctionCall *f = dynamic_cast<FunctionCall *>(instruction);
                writeToFile("    " + f->function_name + "(");
                for (size_t i = 0; i < f->arguments.size(); ++i) {
                    if (i > 0) {
                        writeToFile(", ");
                    }
                    writeToFile(f->arguments[i]->literal_value);
                }
                writeToFile(");\n");
                break;
            }
            case NodeType::RETURN_STATEMENT: {
                ReturnStatement *ret =
                    dynamic_cast<ReturnStatement *>(instruction);
                if (ret->returned_value->left_operand != nullptr) {
                    writeToFile(
                        "    return " +
                        ret->returned_value->left_operand->variable_name + " " +
                        operationToString(ret->returned_value->operation) +
                        " " +
                        ret->returned_value->right_operand->literal_value);
                } else if (ret->returned_value->variable_reference != nullptr) {
                    writeToFile("    return " +
                                ret->returned_value->variable_reference->name);
                } else {
                    increaseIndentation();
                    writeTabs();
                    writeToFile("return ");
                    writeToFile(ret->returned_value->literal_value);
                }
                writeToFile(";\n");
                break;
            }
            case NodeType::IF: {
                IfStatement *if_ = dynamic_cast<IfStatement *>(instruction);
                writeToFile("\tif(");
                writeToFile(if_->condition->left_operand->literal_value);
                writeToFile(" ");
                writeToFile(operationToString(if_->condition->operation));
                writeToFile(" ");
                writeToFile(if_->condition->right_operand->literal_value);
                writeToFile(")");
                writeToFile("{\n");
                increaseIndentation();
                writeTabs();
                writeIFBody(*if_->ifBody);
                decreaseIndentation();
                writeTabs();
                writeToFile("}\n");
                decreaseIndentation();
                break;
            }
            case NodeType::ELSE: {
                ElseStatement *else_ =
                    dynamic_cast<ElseStatement *>(instruction);
                writeToFile("\telse");
                writeToFile("{\n");
                increaseIndentation();
                writeIFBody(*else_->elseBody);
                decreaseIndentation();
                writeTabs();
                decreaseIndentation();
                writeToFile("}\n");
                break;
            }
            case NodeType::PRINT_NODE: {
                PrintNode *printNode = dynamic_cast<PrintNode *>(instruction);
                increaseIndentation();
                writeTabs();
                writeToFile("println(");

                for (size_t i = 0; i < printNode->arguments.size(); ++i) {
                    if (i > 0) {
                        writeToFile(", ");
                    }

                    writeToFile("\"");
                    writeToFile(printNode->arguments[i]);
                    writeToFile("\"");
                }

                writeToFile(");\n");
                decreaseIndentation();
                break;
            }

            default:
                break;
            }
        }
    }

    void writeIFBody(const FunctionBody &body) {
        writeFunctionBody(body.getInstructions());
    }

    void GenIR() {
        if (astGen.nodes.empty()) {
            std::cerr << "AST is empty" << std::endl;
            return;
        }

        writeIncludes();

        for (auto &node : astGen.nodes) {
            switch (node->type) {
            case NodeType::FUNCTION_DECLARATION: {
                FunctionDeclaration *func =
                    dynamic_cast<FunctionDeclaration *>(node);

                writeToFile(dataTypeToCType(func->return_type) + " " +
                            func->name + "(");

                for (size_t i = 0; i < func->parameters.size(); ++i) {
                    if (i > 0) {
                        writeToFile(", ");
                    }
                    writeToFile(
                        dataTypeToCType(func->parameters[i]->variable_type) +
                        " " + func->parameters[i]->name);
                }

                writeToFile(") {\n");

                writeFunctionBody(func->body->getInstructions());
                writeToFile("}\n");
                fflush(outputFile);
                break;
            }
            default:
                break;
            }
        }
    }

private:
    std::string inputFileName;
    std::string outputFileName;
    FILE *inputFile;
    FILE *outputFile;
    ASTGen &astGen;
    Parser &parser;
    int indentationLevel;
};
