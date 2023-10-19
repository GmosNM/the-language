#include "XIR.hpp"
#include "parser.hpp"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " file_name" << std::endl;
        return 1;
    }
    Lexer lexer(argv[1]);
    Parser parser(lexer);
    parser.parse();

    IR ir(argv[1], "output.c", parser.ast, parser);
    ir.GenIR();

    return 0;
}
