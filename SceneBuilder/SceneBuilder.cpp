#include <iostream>
#include <fstream>
#include <sstream>
#include "Scanner.h"
#include "SyntaxError.h"
#include "Parser.h"


int main(int argc, char* argv[])
{
    if (argc < 2)
        return -1;

    if (strcmp(argv[1], "-fin") == 0 && argc >= 3) {
        std::ifstream infile;
        std::string filename = argv[2];
        infile.open(filename);
        Scanner scanner(infile);
        Parser parser(scanner);
        parser.parse();
        infile.close();
    }
}
