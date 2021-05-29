#include <iostream>
#include <fstream>
#include <sstream>
#include "Scanner/Scanner.h"
#include "Exceptions/SyntaxError.h"
#include "Parser/Parser.h"
#include "Analizer/Analizer.h"
#include "Generator/Generator.h"


int main(int argc, char* argv[]) {

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
            try {
                Parser parser(scanner);
                auto result = parser.parse();
                if (!result) {
                    std::cout << "Empty source file." << std::endl;
                    return 0;
                }
                Analizer analizer(result.get());
                if (analizer.isValid())
                {
                    Generator generator(std::move(result));
                    if (generator.generate("App.exe")) 
                    { //run program if files sucessfuly generated
                        generator.run();
                    }
                }
                infile.close();
            }
            catch (const SceneBuilderException& er) {
                std::cout << "Syntax Eror " << er.what([&scanner](const Position& pos) {
                    return scanner.getLineError(pos);
                    }) << std::endl;
            }

        }

    }
    catch (const std::runtime_error& er) {
        std::cout << "error" << er.what() << std::endl;
    }
    catch (...) {
        std::cout << "Undefined error" << std::endl;
        throw;
    }
    return 0;
}
