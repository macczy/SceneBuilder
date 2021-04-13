#include <iostream>
#include <fstream>
#include "Scanner.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
        return -1;

    std::ifstream infile;
    if (strcmp(argv[1], "-fin") == 0 && argc >= 3) {
        std::string filename = argv[2];
        infile.open(filename);
        Scanner scanner(infile);
        while (scanner.getToken().getType() != Token::TokenType::END_OF_FILE) {
            //if(scanner.getToken().getType() == Token::TokenType::HEX_CONST)
                std::cout << scanner.getToken().getValue() << '\n';
            try {
                scanner.next();
            }
            catch (const std::runtime_error& error) {
                std::cout << error.what() << '\n';
            }
        }
        infile.close();
    }
}
