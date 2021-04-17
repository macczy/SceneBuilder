#include <iostream>
#include <fstream>
#include <sstream>
#include "Scanner.h"
#include "SyntaxError.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
        return -1;

    if (strcmp(argv[1], "-fin") == 0 && argc >= 3) {
        std::ifstream infile;
        std::ifstream infile2;
        std::string filename = argv[2];
        infile.open(filename);
        infile2.open(filename);

        Scanner scanner(infile);
        while (scanner.getToken().getType() != Token::TokenType::END_OF_FILE) {
            std::cout << scanner.getToken().getValue() << '\n';
            try {
                scanner.next();
            }
            catch (const SyntaxError& error) {
                std::cout << error.what() << ":\n" << error.visit(infile2);
            }
        }
        std::cout << std::endl;
        infile.close();
        infile2.close();
    }
}
