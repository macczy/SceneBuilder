#include <iostream>
#include <fstream>
#include <sstream>
#include "Scanner/Scanner.h"
#include "Exceptions/SyntaxError.h"
#include "Parser/Parser.h"
#include "Analizer/Analizer.h"
#include "Generator/Generator.h"
#include <algorithm>

int main(int argc, char* argv[]) {

    try {
        if (argc < 2)
            return -1;

        std::ifstream infile;
        if (strcmp(argv[1], "-fin") == 0 && argc >= 3) {
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
                std::cout << "Error: " << er.what([&infile](const Position& pos) {
                    std::string buffer;
                    auto positionInInput = infile.tellg();
                    std::streamoff seekedPosition = pos.totalPositionNumber - pos.columnNumber;
                    infile.clear();
                    infile.seekg(seekedPosition > 0 ? seekedPosition : 0, std::ios::beg);
                    std::getline(infile, buffer);
                    std::replace(buffer.begin(), buffer.end(), '\t', ' ');
                    auto spaceLength = pos.columnNumber - 1;
                    buffer = "\n" + buffer + "\n" + std::string(spaceLength > 0 ? spaceLength : 0, ' ') + "^";
                    infile.seekg(positionInInput, std::ios::beg);
                    return buffer;
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
