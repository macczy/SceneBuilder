#include "Parser.h"
#include "../SyntaxError.h"
#include <optional>

using TokenType = Token::TokenType;


Parser::Parser(Scanner& scanner) : scanner(scanner), root(new SceneRoot()) {
    knownTypes = {"Circle", "Line", "Rectangle", "Polygon"};
    currentToken = scanner.getToken();
}

const Token& Parser::getNextToken() {
    scanner.next();
    currentToken = scanner.getToken();
    return currentToken;
}

bool Parser::tryBuildComplexObject() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) return false;


    return true;
}

bool Parser::tryBuildVariableObject() {
    if (currentToken.getType() != TokenType::VARIABLE_IDENTIFIER) return false;
    if (getNextToken().getType() != TokenType::EQUAL_SIGN) return false;
    if (!tryBuildComplexOrSimpleObject()) {
        return false;
    }
    return true;
}

bool Parser::isKnownType(const std::string& value) {
    return std::find(knownTypes.begin(), knownTypes.end(), value) != knownTypes.end();
}

bool Parser::tryBuildKnownType() {
    throw SyntaxError("Unimplemented method " + currentToken.getValue() + " " + currentToken.getPosition().toString());
}


bool Parser::tryBuildComplexOrSimpleObject() {
    if (getNextToken().getType() != TokenType::TYPE_IDENTIFIER) return false;
    if (getNextToken().getType() != TokenType::OPENING_BRACE) return false;
    if (isKnownType(getNextToken().getValue())) {
        return tryBuildKnownType();
    }
    throw SyntaxError("Unknown type " + currentToken.getValue() + " " + currentToken.getPosition().toString());
}

bool Parser::tryBuildComplexObjectDeclaration() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) return false;
    if (getNextToken().getType() != TokenType::EQUAL_SIGN) return false;
    if (getNextToken().getType() != TokenType::OPENING_BRACE) return false;
    getNextToken();
    do {
        if (!tryBuildVariableObject()) {
            return false;
        }
    } while (currentToken.getType() != TokenType::CLOSING_BRACE);
    return true;
}

bool Parser::tryBuildAnimationDeclaration() {
    if (currentToken.getType() != TokenType::VARIABLE_IDENTIFIER) return false;


    return true;
}

std::string Parser::errorExpectingXgotYat(const std::string& expected, const std::string& got, const Position& pos) {
    return "Expected " + expected + ", but got " + got + " " + pos.toString() + "\n" + scanner.getLineError(pos);
}


std::optional<Color> Parser::tryBuildColor() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER || currentToken.getValue() != "Color") return std::nullopt;
    Position startingColorPosition = currentToken.getPosition();
    if (getNextToken().getType() != TokenType::OPENING_BRACKET) throw SyntaxError(errorExpectingXgotYat("'('", currentToken.getValue(), currentToken.getPosition()));
    if (getNextToken().getType() != TokenType::HEX_CONST) throw SyntaxError(errorExpectingXgotYat("hexadecimal value", currentToken.getValue(), currentToken.getPosition()));
    auto firstHexValue = currentToken.getValue();
    if (getNextToken().getType() != TokenType::COMMA) throw SyntaxError(errorExpectingXgotYat("','", currentToken.getValue(), currentToken.getPosition()));
    if (getNextToken().getType() != TokenType::HEX_CONST) throw SyntaxError(errorExpectingXgotYat("hexadecimal value", currentToken.getValue(), currentToken.getPosition()));
    auto secondHexValue = currentToken.getValue();
    if (getNextToken().getType() != TokenType::COMMA) throw SyntaxError(errorExpectingXgotYat("','", currentToken.getValue(), currentToken.getPosition()));
    if (getNextToken().getType() != TokenType::HEX_CONST) throw SyntaxError(errorExpectingXgotYat("hexadecimal value", currentToken.getValue(), currentToken.getPosition()));
    auto thirdHexValue = currentToken.getValue();
    if (getNextToken().getType() != TokenType::CLOSING_BRACKET) throw SyntaxError(errorExpectingXgotYat("')'", currentToken.getValue(), currentToken.getPosition()));
    return Color(startingColorPosition, firstHexValue, secondHexValue, thirdHexValue);
}

std::optional<std::string> Parser::getDecimalValue() {
    if (currentToken.getType() == TokenType::MINUS) {
        if (getNextToken().getType() != Token::TokenType::DECIMAL_CONST) throw SyntaxError(errorExpectingXgotYat("decimal value", currentToken.getValue(), currentToken.getPosition()));
        return "-" + currentToken.getValue();
    }
    else if (currentToken.getType() == TokenType::DECIMAL_CONST)
        return currentToken.getValue(); 
    return std::nullopt;
}

std::optional<Point> Parser::tryBuildPoint() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER || currentToken.getValue() != "Point") return std::nullopt;
    Position startingPointPosition = currentToken.getPosition();
    if (getNextToken().getType() != TokenType::OPENING_BRACKET) throw SyntaxError(errorExpectingXgotYat("'('", currentToken.getValue(), currentToken.getPosition()));
    if (getNextToken().getType() == TokenType::CLOSING_BRACKET) return Point(startingPointPosition);

    auto firstDecimalValue = getDecimalValue();
    if (!firstDecimalValue.has_value()) throw SyntaxError(errorExpectingXgotYat("')' or decimal value", currentToken.getValue(), currentToken.getPosition()));

    if (getNextToken().getType() != TokenType::COMMA) {
        if (currentToken.getType() == TokenType::CLOSING_BRACKET) return Point(startingPointPosition, firstDecimalValue.value());
        throw SyntaxError(errorExpectingXgotYat("',' or ')'", currentToken.getValue(), currentToken.getPosition()));
    }
    getNextToken();
    auto secondDecimalValue = getDecimalValue();
    if (!secondDecimalValue) throw SyntaxError(errorExpectingXgotYat("decimal value", currentToken.getValue(), currentToken.getPosition()));

    if (getNextToken().getType() != TokenType::COMMA) {
        if (currentToken.getType() == TokenType::CLOSING_BRACKET) return Point(startingPointPosition, firstDecimalValue.value());
        throw SyntaxError(errorExpectingXgotYat("',' or ')'", currentToken.getValue(), currentToken.getPosition()));
    }
    getNextToken();
    auto thirdDecimalValue = getDecimalValue();
    if (!thirdDecimalValue) throw SyntaxError(errorExpectingXgotYat("decimal value", currentToken.getValue(), currentToken.getPosition()));
        
    if (getNextToken().getType() != TokenType::CLOSING_BRACKET) throw SyntaxError(errorExpectingXgotYat("')'", currentToken.getValue(), currentToken.getPosition()));

    return Point(startingPointPosition, firstDecimalValue.value(), secondDecimalValue.value(), thirdDecimalValue.value());
}

std::unique_ptr<SceneRoot> Parser::parse() {
    try {
        while (currentToken.getType() != TokenType::END_OF_FILE) {
            if (tryBuildComplexObjectDeclaration()) {
                std::cout << "here";
            }
            else if (tryBuildAnimationDeclaration());
            else {
                std::string errorMessage = "\nExpected Complex Object declaration or Animation declaration but got '" + currentToken.getValue()
                    + "' " + currentToken.getPosition().toString() + scanner.getLineError(currentToken.getPosition());
                std::cout << errorMessage << '\n';
                std::cout << currentToken.getType();
                return nullptr;
            }
            try {
                scanner.next();
            } catch (const SyntaxError& error) {
                std::cout << error.what() << '\n';
            }
            currentToken = scanner.getToken();
        }
    }
    catch (SyntaxError er) {
        std::cout << er.what();
        std::string errorMessage = "\nGot '" + currentToken.getValue()
            + "' " + currentToken.getPosition().toString() + scanner.getLineError(currentToken.getPosition());
        std::cout << errorMessage << std::endl;
    }
    catch (...) {
        std::cout << "Unknown exception" << std::endl;
    }
    std::cout << std::endl;
	return std::move(root);
}