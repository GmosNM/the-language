#include "Parser.hpp"
#include "astGen.hpp"

class IR {
public:
    IR(std::string inputFileName, std::string outputFileName, ASTGen &astGen,
       Parser &parser)
        : inputFileName(inputFileName), outputFileName(outputFileName),
          astGen(astGen), parser(parser) {
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
                }
                if (ret->returned_value->variable_reference != nullptr) {
                    writeToFile("    return " +
                                ret->returned_value->variable_reference->name);
                }
                writeToFile(";\n");
                break;
            }
            default:
                break;
            }
        }
    }

    void GenIR() {
        if (astGen.nodes.empty()) {
            std::cerr << "AST is empty" << std::endl;
            return;
        }

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

                // Write the function body instruction by instruction
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
};
