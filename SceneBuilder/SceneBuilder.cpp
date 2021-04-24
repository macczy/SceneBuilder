#include <iostream>
#include <fstream>
#include <sstream>
#include "Scanner/Scanner.h"
#include "SyntaxError.h"
#include "Parser/Parser.h"


int main(int argc, char* argv[])
{
    try {
        if (argc < 2)
            return -1;

        if (strcmp(argv[1], "-fin") == 0 && argc >= 3) {
            std::ifstream infile;
            std::string filename = argv[2];
            infile.open(filename);
            if (!infile.is_open()) {
                throw std::runtime_error("File " + std::string(argv[2]) + "could not be opened");
            }
            SceneBuilderScanner scanner(infile);
            Parser parser(scanner);
            parser.parse();
            infile.close();
        }
    }
    catch (const SyntaxError& er) {
        std::cout << er.what() << std::endl;
    }
    catch (...) {
        std::cout << "Undefined error" << std::endl;
    }
}
