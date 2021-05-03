#include "Parser.h"
#include "../SyntaxError.h"
#include <optional>
#include <functional>

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

std::optional<Color> Parser::tryBuildColor() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER || currentToken.getValue() != "Color") 
        return std::nullopt;
    Position startingColorPosition = currentToken.getPosition();
    if (getNextToken().getType() != TokenType::OPENING_BRACKET) 
        throwSyntaxError("'('", currentToken.getValue(), currentToken);
    if (getNextToken().getType() != TokenType::HEX_CONST) 
        throwSyntaxError("hexadecimal value", currentToken.getValue(), currentToken);
    auto firstHexValue = currentToken.getValue();
    if (getNextToken().getType() != TokenType::COMMA) 
        throwSyntaxError("','", currentToken.getValue(), currentToken);
    if (getNextToken().getType() != TokenType::HEX_CONST) 
        throwSyntaxError("hexadecimal value", currentToken.getValue(), currentToken);
    auto secondHexValue = currentToken.getValue();
    if (getNextToken().getType() != TokenType::COMMA) 
        throwSyntaxError("','", currentToken.getValue(), currentToken);
    if (getNextToken().getType() != TokenType::HEX_CONST) 
        throwSyntaxError("hexadecimal value", currentToken.getValue(), currentToken);
    auto thirdHexValue = currentToken.getValue();
    if (getNextToken().getType() != TokenType::CLOSING_BRACKET) 
        throwSyntaxError("')'", currentToken.getValue(), currentToken);
    return Color(startingColorPosition, firstHexValue, secondHexValue, thirdHexValue);
}

std::optional<DecimalValue> Parser::tryBuildDecimalValue() {
    Position pos = currentToken.getPosition();
    if (currentToken.getType() == TokenType::MINUS) {
        if (getNextToken().getType() != Token::TokenType::DECIMAL_CONST) 
            throwSyntaxError("decimal value", currentToken.getValue(), currentToken);
        return DecimalValue(pos, "-" + currentToken.getValue());
    }
    else if (currentToken.getType() == TokenType::DECIMAL_CONST)
        return DecimalValue(pos, currentToken.getValue());
    return std::nullopt;
}

void Parser::throwSyntaxError(const std::string& expected, const std::string& got, Token& token) {
    throw SyntaxError(expected, got, token.getPosition(), [=](const Position& pos) {
        return scanner.getLineError(pos);
    });
}


std::optional<Identifier> Parser::tryBuildIdentifier() {
    if (currentToken.getType() != TokenType::VARIABLE_IDENTIFIER)
        return std::nullopt;

    std::vector<std::pair<Position, std::string>> identifiers;
    identifiers.push_back(std::make_pair(currentToken.getPosition(), currentToken.getValue()));

    while (getNextToken().getType() == TokenType::DOT) {
        if (getNextToken().getType() != TokenType::VARIABLE_IDENTIFIER)
            throwSyntaxError("variable identifier after '.'", currentToken.getValue(), currentToken);
        identifiers.push_back(std::make_pair(currentToken.getPosition(), currentToken.getValue()));
    }
    if(identifiers.size() == 1)
        return Identifier(identifiers[0].first, identifiers[0].second);

    auto identifier_ptr = std::make_unique<Identifier>(identifiers.back().first, identifiers.back().second);
    identifiers.pop_back();
    while (identifiers.size() > 1) {
        identifier_ptr = std::make_unique<Identifier>(identifiers.back().first, identifiers.back().second, std::move(identifier_ptr));
        identifiers.pop_back();
    }
    return Identifier(identifiers[0].first, identifiers[0].second, std::move(identifier_ptr));
}

std::optional<Point> Parser::tryBuildPoint() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER || currentToken.getValue() != "Point") 
        return std::nullopt;
    Position startingPointPosition = currentToken.getPosition();
    if (getNextToken().getType() != TokenType::OPENING_BRACKET)
        throwSyntaxError("'('", currentToken.getValue(), currentToken);
    if (getNextToken().getType() == TokenType::CLOSING_BRACKET)
        return Point(startingPointPosition);

    auto firstDecimalValue = tryBuildDecimalValue();
    if (!firstDecimalValue.has_value()) 
        throwSyntaxError("')' or decimal value", currentToken.getValue(), currentToken);

    if (getNextToken().getType() != TokenType::COMMA) {
        if (currentToken.getType() == TokenType::CLOSING_BRACKET) 
            return Point(startingPointPosition, firstDecimalValue.value());
        throwSyntaxError("',' or ')'", currentToken.getValue(), currentToken);
    }
    getNextToken();
    auto secondDecimalValue = tryBuildDecimalValue();
    if (!secondDecimalValue)
        throwSyntaxError("decimal value", currentToken.getValue(), currentToken);

    if (getNextToken().getType() != TokenType::COMMA) {
        if (currentToken.getType() == TokenType::CLOSING_BRACKET) 
            return Point(startingPointPosition, firstDecimalValue.value());
        throwSyntaxError("',' or ')'", currentToken.getValue(), currentToken);
    }
    getNextToken();
    auto thirdDecimalValue = tryBuildDecimalValue();
    if (!thirdDecimalValue) throwSyntaxError("decimal value", currentToken.getValue(), currentToken);
        
    if (getNextToken().getType() != TokenType::CLOSING_BRACKET) throwSyntaxError("')'", currentToken.getValue(), currentToken);

    return Point(startingPointPosition, firstDecimalValue.value(), secondDecimalValue.value(), thirdDecimalValue.value());
}

std::optional<Value> Parser::tryBuildValue() {
    if (auto value = tryBuildDecimalValue(); value.has_value())
        return value;
    if (auto point = tryBuildPoint(); point.has_value())
        return point;
    if (auto color = tryBuildColor(); color.has_value())
        return color;
    if (auto identifier = tryBuildIdentifier(); identifier.has_value())
        return identifier;
    return std::nullopt;
}

std::optional<Addition> Parser::tryBuildAddition( Value& firstValue) {
    if (!(currentToken.getType() == TokenType::PLUS || currentToken.getType() == TokenType::MINUS) )
        return std::nullopt;
    Addition::Operator oper = currentToken.getType() == TokenType::PLUS ? Addition::Operator::PLUS : Addition::Operator::MINUS;
    auto operStr = currentToken.getValue();
    getNextToken();
    if (auto secondValue = tryBuildValue(); secondValue.has_value()) {
         return Addition(getValuePosition(firstValue), 
            std::make_unique<Value>(std::move(firstValue)), std::make_unique<Value>(std::move(secondValue.value())), oper);
    }
    throwSyntaxError("value after " + operStr, currentToken.getValue(), currentToken);
    return std::nullopt;
}

std::optional<Multiplication> Parser::tryBuildMultiplication(Value& firstValue) {
    return std::nullopt;
}

//DisjunctionExpression ConjuctionExpression
std::optional<LogicalExpression> Parser::tryBuildLogicalExpression(LogicSubExpression& firstValue) {
    return std::nullopt;
}


std::optional<DisjunctionExpression> Parser::tryBuildDisjunctionExpression(LogicSubExpression& firstValue) {
    return std::nullopt;
}
std::optional<ConjuctionExpression> Parser::tryBuildConjuctionExpression(LogicSubExpression& firstValue) {
    return std::nullopt;
}

std::optional<Comparison> Parser::tryBuildComparison(Value& firstValue) {
    auto comparisonToken = currentToken;
    if(!ComparisonFactory::isComparisonOperator(currentToken.getType()))
        return std::nullopt;
    auto nextToken = getNextToken();
    if (auto secondValue = tryBuildValue(); secondValue.has_value()) {
        return ComparisonFactory::getComparison(getValuePosition(firstValue), std::make_unique<Value>(std::move(firstValue)), 
            std::make_unique<Value>(std::move(secondValue.value())), comparisonToken.getType());
    }
    throwSyntaxError("value after " + comparisonToken.getValue(), nextToken.getValue(), nextToken);
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
    }
    catch (...) {
        std::cout << "Unknown exception" << std::endl;
    }
    std::cout << std::endl;
	return std::move(root);
}