#include "Parser.h"
#include "SyntaxError.h"
#include <optional>

typedef Token::TokenType TokenType;


bool Parser::tryBuildComplesObject() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) return false;


    return true;
}

bool Parser::tryBuildAnimationDeclaration() {
    if (currentToken.getType() != TokenType::VARIABLE_IDENTIFIER) return false;


    return true;
}

std::unique_ptr<SceneRoot> Parser::parse() {
    currentToken = scanner.getToken();
    while (currentToken.getType() != TokenType::END_OF_FILE) {
        if (tryBuildComplesObject());
        else if (tryBuildAnimationDeclaration());
        else {
            std::string errorMessage = "Expected Complex Object declaration or Animation declaration but got '" + currentToken.getValue()
                + "' " + currentToken.getPosition().toString() + scanner.getLineError(currentToken.getPosition());
            std::cout << errorMessage << '\n';
            return nullptr;
        }
        try {
            scanner.next();
        } catch (const SyntaxError& error) {
            std::cout << error.what() << '\n';
        }
        currentToken = scanner.getToken();
    }
    std::cout << std::endl;
	return std::move(root);
}