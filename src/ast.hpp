#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct DataType {
    enum class Category { INT, FLOAT, BOOL, CHAR, STRING, UNKNOWN };

    Category category;

    DataType() : category(Category::UNKNOWN) {}
    DataType(Category c) : category(c) {}
};

enum class Operation { ADD, SUBTRACT, MULTIPLY, DIVIDE, EQUAL };

enum class NodeType {
    // Variables
    VARIABLE_DECLARATION,
    VARIABLE_REFERENCE,
    VARIABLE_ASSIGNMENT,
    // Functions
    FUNCTION_DECLARATION,
    FUNCTION_BODY,
    FUNCTION_CALL,
    PRINT_NODE,
    // Statements
    RETURN_STATEMENT,
    // expressions
    EXPRESSION,
    IF,
    ELSE
};

std::string nodeTypeToString(NodeType type);
std::string dataTypeToString(DataType type);
std::string operationToString(Operation op);
std::string dataTypeToCType(DataType type);

struct Instruction {
public:
    NodeType type;
    virtual ~Instruction() {}

    const auto getChildren() const { return children; }

protected:
    std::vector<Instruction *> children;
    Instruction(NodeType t) : type(t) {}
};

struct Expression : public Instruction {
    enum class Type {
        LITERAL,
        VARIABLE,
        BINARY_OPERATION,
        FUNCTION_CALL,
        VARIABLE_REFERENCE,
        VARIABLE_ASSIGNMENT,
        EQUAL_OPERATION,
        PRINT
    };

    Type type;

    // Literal expression constructor
    Expression(std::string value, DataType type)
        : Instruction(NodeType::EXPRESSION), type(Type::LITERAL),
          literal_value(value), variable_type(type) {}

    // Binary operation expression constructor
    Expression(Operation op, Expression *left, Expression *right)
        : Instruction(NodeType::EXPRESSION), type(Type::BINARY_OPERATION),
          operation(op), left_operand(left), right_operand(right) {}

    // Function call expression constructor
    Expression(const std::string &func_name, std::vector<Expression *> args,
               DataType type_)
        : Instruction(NodeType::EXPRESSION), type(Type::FUNCTION_CALL),
          function_name(func_name), arguments(args), variable_type(type_) {}

    // Variable expression constructor
    Expression(const std::string &value, DataType type, std::string name)
        : Instruction(NodeType::EXPRESSION), type(Type::VARIABLE),
          literal_value(value), variable_type(type), variable_name(name) {}

    // Variable reference
    Expression(const std::string &name, struct VariableReference *ref,
               DataType type_, const std::string &value)
        : Instruction(NodeType::EXPRESSION), type(Type::VARIABLE_REFERENCE),
          variable_reference(ref), variable_name(name), variable_type(type_),
          literal_value(value) {}

    // Variable assignment
    Expression(const std::string &var_name, const std::string &new_value,
               DataType type)
        : Instruction(NodeType::EXPRESSION), type(Type::VARIABLE_ASSIGNMENT),
          variable_name(var_name), literal_value(new_value),
          variable_type(type) {}

    // Equal operation
    Expression(Expression *left, Expression *right)
        : Instruction(NodeType::EXPRESSION), type(Type::EQUAL_OPERATION),
          left_operand(left), right_operand(right) {}

    // Print node
    Expression(const std::string &functionName_, std::vector<Expression *> args)
        : Instruction(NodeType::EXPRESSION), type(Type::PRINT),
          function_name(functionName_), arguments(args) {}

    // bool expression
    Expression(bool value)
        : Instruction(NodeType::EXPRESSION), type(Type::LITERAL),
          literal_value(value ? "true" : "false"),
          variable_type(DataType::Category::BOOL) {}

    std::string literal_value;
    std::string variable_name;
    DataType variable_type;
    Operation operation;
    std::unique_ptr<Expression> left_operand;
    std::unique_ptr<Expression> right_operand;
    std::string function_name;
    std::vector<Expression *> arguments;
    VariableReference *variable_reference = nullptr;
};

struct VariableDeclaration : public Instruction {
    std::string name;
    DataType variable_type;
    Expression *initialization_value;

    VariableDeclaration(const std::string &n, DataType type, Expression *init)
        : Instruction(NodeType::VARIABLE_DECLARATION), name(n),
          variable_type(type), initialization_value(init) {}
};

struct VariableReference : public Instruction {
    std::string name;
    DataType variable_type;
    Expression *initialization_value;

    VariableReference(const std::string &n, Expression *value, DataType type)
        : Instruction(NodeType::VARIABLE_REFERENCE), name(n),
          variable_type(type), initialization_value(value) {}
};

struct FunctionDeclaration : public Instruction {
    std::string name;
    std::vector<VariableDeclaration *> parameters;
    DataType return_type;
    struct FunctionBody *body;

    FunctionDeclaration(const std::string &n,
                        const std::vector<VariableDeclaration *> &p, DataType r,
                        FunctionBody *b)
        : Instruction(NodeType::FUNCTION_DECLARATION), name(n), parameters(p),
          return_type(r), body(b) {}
};

struct FunctionCall : public Expression {
    std::vector<Expression *> arguments;
    DataType variable_type;
    std::string function_name;

    FunctionCall(const std::string &n, std::vector<Expression *> args,
                 DataType type)
        : Expression(n, args, type), arguments(args), variable_type(type) {}
};

struct ReturnStatement : public Instruction {
    Expression *returned_value;

    ReturnStatement(Expression *rv)
        : Instruction(NodeType::RETURN_STATEMENT), returned_value(rv) {}
};

struct FunctionBody : public Instruction {
public:
    FunctionBody() : Instruction(NodeType::FUNCTION_BODY) {}

    const auto getInstructions() const { return instructions; }

    void addInstruction(Instruction *instruction) {
        instructions.push_back(instruction);
    }

    auto addReturnStatement(Expression *rv) {
        auto ret = new ReturnStatement(rv);
        addInstruction(ret);
        return ret;
    }

    auto getReturnType() { return returnType; }

    auto setReturnType(DataType type) { returnType = type; }

protected:
    std::vector<Instruction *> instructions;
    DataType returnType;
};

struct VariableAssignment : public Instruction {
    std::string name;
    Expression *new_value;
    DataType variable_type;

    VariableAssignment(const std::string &n, Expression *v, DataType type)
        : Instruction(NodeType::VARIABLE_ASSIGNMENT), name(n), new_value(v),
          variable_type(type) {}
};

struct IfStatement : public Instruction {
    Expression *condition;
    FunctionBody *ifBody;
    FunctionBody *elseBody;

    IfStatement(Expression *cond, FunctionBody *ifBody, FunctionBody *elseBody)
        : Instruction(NodeType::IF), condition(cond), ifBody(ifBody),
          elseBody(elseBody) {}

    IfStatement(Expression *cond, FunctionBody *ifBody)
        : Instruction(NodeType::IF), condition(cond), ifBody(ifBody) {}
};

struct ElseStatement : public Instruction {
    FunctionBody *elseBody;
    FunctionBody *ifBody;

    ElseStatement(FunctionBody *elseBody, FunctionBody *ifBody)
        : Instruction(NodeType::ELSE), elseBody(elseBody), ifBody(ifBody) {}
};

struct PrintNode : public Instruction {
    std::string function_name;
    std::vector<Expression *> arguments2;
    std::vector<std::string> arguments;

    PrintNode(const std::string &name, const std::vector<std::string> args,
              const std::vector<Expression *> &args2)
        : Instruction(NodeType::PRINT_NODE), function_name(name),
          arguments(args), arguments2(args2) {}
};
