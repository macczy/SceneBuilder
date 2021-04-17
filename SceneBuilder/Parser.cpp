#include "Parser.h"
#include "SyntaxError.h"

SceneRoot Parser::parse() {
    while (scanner.getToken().getType() != Token::TokenType::END_OF_FILE) {
       // std::cout << scanner.getToken().getValue() << '\n';
        try {
            scanner.next();
        }
        catch (const SyntaxError& error) {
            std::cout << error.what() << '\n';
        }
    }
    std::cout << std::endl;
	return root;
}