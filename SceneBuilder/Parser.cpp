#include "Parser.h"
#include "SyntaxError.h"

typedef Token::TokenType TokenType;


bool Parser::tryBuildComplesObject() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) return false;


    return true;
}

bool Parser::tryBuildAnimationDeclaration() {
    if (currentToken.getType() != TokenType::VARIABLE_IDENTIFIER) return false;


    return true;
}

SceneRoot Parser::parse() {
    currentToken = scanner.getToken();
    while (currentToken.getType() != TokenType::END_OF_FILE) {
        try {

            if (tryBuildComplesObject());
            else if (tryBuildAnimationDeclaration());
            else {
                std::string errorMessage = "Expected Complex Object declaration or Animation declaration but got '" + currentToken.getValue()
                    + "' " + currentToken.getPosition().toString() + "\n" + scanner.getLineError(currentToken.getPosition());
                std::cout << errorMessage << '\n';
            }
            scanner.next();
            currentToken = scanner.getToken();
        }
        catch (const SyntaxError& error) {
            std::cout << error.what() << '\n';
        }
    }
    std::cout << std::endl;
	return root;
}