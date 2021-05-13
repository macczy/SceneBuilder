#include "Parser.h"
#include "../SyntaxError.h"
#include <optional>
#include <functional>

using TokenType = Token::TokenType;

Parser::Parser(Scanner& scanner) : scanner(scanner), root(std::make_unique<SceneRoot>()) {
    currentToken = scanner.getToken();
}

const Token& Parser::getNextToken() {
    scanner.next();
    currentToken = scanner.getToken();
    return currentToken;
}

ComplexObjectDeclarationPtr Parser::tryBuildNewComplexObject(const std::string& name) {//'Identifier =' should already be found by more general function, this only checks what's after
    if (currentToken.getType() != TokenType::OPENING_BRACE) return nullptr;
    auto pos = currentToken.getPosition();
    Objects objects;
    Properties properties;
    getNextToken();
    while (auto ident = tryBuildIdentifier()) {
        if (auto property = tryBuildProperty(*ident); property) {
            properties.push_back(std::move(property));
        }
        else {
            if (currentToken.getType() != TokenType::EQUAL_SIGN) throwSyntaxError("= or :", currentToken.getValue(), currentToken);
            getNextToken();
            if (auto obj = tryBuildBasicObject()) {
                objects.push_back(std::make_unique<ObjectIdentifierPair>(*ident, obj));
            }
            else if (auto obj = tryBuildComplexObject()) {
                objects.push_back(std::make_unique<ObjectIdentifierPair>(*ident, obj));
            }
            else {//try build animation
                throwSyntaxError("object declaration", currentToken.getValue(), currentToken);
            }
        }
    }
    if (currentToken.getType() != TokenType::CLOSING_BRACE) throwSyntaxError("closing bracket or identifier", currentToken.getValue(), currentToken);
    getNextToken();
    return std::make_unique<ComplexObjectDeclaration>(name, pos, properties, objects);
}

bool Parser::isKnownType(const std::string& value) {
    return std::find_if(knownTypes.begin(), knownTypes.end(), [&value](const auto& val) {
        return value == val->getName();
     }) != knownTypes.end();
}

AnimationDeclarationPtr Parser::tryBuildAnimationDeclaration() {
    //throw std::runtime_error("not implemented");
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) return nullptr;
    if(currentToken.getValue() != "AnimationSequence") return nullptr;


    return nullptr;
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
    getNextToken();
    return Color(startingColorPosition, firstHexValue, secondHexValue, thirdHexValue);
}

std::optional<DecimalValue> Parser::tryBuildDecimalValue() {
    Position pos = currentToken.getPosition();
    if (currentToken.getType() == TokenType::MINUS) {
        if (getNextToken().getType() != Token::TokenType::DECIMAL_CONST) 
            throwSyntaxError("decimal value", currentToken.getValue(), currentToken);
        auto value = currentToken.getValue();
        getNextToken();
        return DecimalValue(pos, "-"+value);
    }
    else if (currentToken.getType() == TokenType::DECIMAL_CONST) {
        auto value = currentToken.getValue();
        getNextToken();
        return DecimalValue(pos, value);
    }
    return std::nullopt;
}

[[noreturn]] void Parser::throwSyntaxError(const std::string& expected, const std::string& got, Token& token) {
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
        identifier_ptr = std::make_unique<Identifier>(identifiers.back().first, identifiers.back().second, identifier_ptr);
        identifiers.pop_back();
    }
    return Identifier(identifiers[0].first, identifiers[0].second, identifier_ptr);
}

std::optional<PointArray> Parser::tryBuildPointArray() {
    if (currentToken.getType() != TokenType::OPENING_SQUARE_BRACE) return std::nullopt;
    auto pos = currentToken.getPosition();
    std::vector<Point> points;
    do {
        getNextToken();
        if (auto point = tryBuildPoint(); point)
            points.push_back(*point);
        else 
            throwSyntaxError("vertex declaration", currentToken.getValue(), currentToken);
    } while (currentToken.getType() == TokenType::COMMA);
    if (currentToken.getType() != TokenType::CLOSING_SQUARE_BRACE) throwSyntaxError("closing square brace", currentToken.getValue(), currentToken);
    getNextToken();
    return PointArray(pos, points);
}

std::optional<Point> Parser::tryBuildPoint() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER || currentToken.getValue() != "Point") 
        return std::nullopt;
    Position startingPointPosition = currentToken.getPosition();
    if (getNextToken().getType() != TokenType::OPENING_BRACKET)
        throwSyntaxError("'('", currentToken.getValue(), currentToken);
    if (getNextToken().getType() == TokenType::CLOSING_BRACKET) {
        getNextToken();
        return Point(startingPointPosition);
    }

    auto firstDecimalValue = tryBuildDecimalValue();
    if (!firstDecimalValue) 
        throwSyntaxError("')' or decimal value", currentToken.getValue(), currentToken);

    if (currentToken.getType() != TokenType::COMMA) {
        if (currentToken.getType() == TokenType::CLOSING_BRACKET) {
            getNextToken();
            return Point(startingPointPosition, *firstDecimalValue);
        }
        throwSyntaxError("',' or ')'", currentToken.getValue(), currentToken);
    }
    getNextToken();
    auto secondDecimalValue = tryBuildDecimalValue();
    if (!secondDecimalValue)
        throwSyntaxError("decimal value", currentToken.getValue(), currentToken);

    if (currentToken.getType() != TokenType::COMMA) {
        if (currentToken.getType() == TokenType::CLOSING_BRACKET) {
            getNextToken();
            return Point(startingPointPosition, *firstDecimalValue);
        }
        throwSyntaxError("',' or ')'", currentToken.getValue(), currentToken);
    }
    getNextToken();
    auto thirdDecimalValue = tryBuildDecimalValue();
    if (!thirdDecimalValue) throwSyntaxError("decimal value", currentToken.getValue(), currentToken);
        
    if (currentToken.getType() != TokenType::CLOSING_BRACKET) throwSyntaxError("')'", currentToken.getValue(), currentToken);

    getNextToken();
    return Point(startingPointPosition, *firstDecimalValue, *secondDecimalValue, *thirdDecimalValue);
}

std::optional<Expression> Parser::tryBuildValue() {
    if (auto identifier = tryBuildIdentifier(); identifier) {
        return identifier;
    }
    if (auto value = tryBuildDecimalValue(); value) {
        return value;
    }
    if (auto point = tryBuildPoint(); point) {
        return point;
    }
    if (auto color = tryBuildColor(); color) {
        return color;
    }
    if (auto points = tryBuildPointArray(); points) {
        return points;
    }
    return std::nullopt;
}

std::optional<Expression> Parser::tryBuildBrackets() {
    if (currentToken.getType() == Token::TokenType::OPENING_BRACKET) {
        auto openingToken = currentToken;
        getNextToken();
        if (auto subExpr = tryBuildExpression(); subExpr) {
            if (currentToken.getType() == Token::TokenType::CLOSING_BRACKET) {
                getNextToken();
                return subExpr;
            }
            else {
                throwSyntaxError("expression", "empty brackets", openingToken);
            }
        }
        else {
            throwSyntaxError("closing bracket", currentToken.getValue(), openingToken);
        }
    }
    return std::nullopt;
}

std::optional<Expression> Parser::tryBuildExpression() {
    if (auto brackets = tryBuildBrackets(); brackets) {
        if (auto expr = tryBuildMultiplicationOrAddition(*brackets); expr) {
            return expr;
        }
        return brackets;
    }
    else if (auto expr = tryBuildSimpleExpression(); expr) {
        if (auto comparison = tryBuildComparison(*expr); comparison) {
            auto subExpr = std::make_unique<LogicalSubExpression>(std::move(comparison));
            if (auto logicalExpr =  tryBuildLogicalExpression(subExpr); logicalExpr) {
                LogicalSubExpressionPtr logicSubExpr = std::make_unique<LogicalSubExpression>(std::move(logicalExpr));
                if (auto ternary = tryBuildTernaryExpression(logicSubExpr); ternary) {
                    return ternary;
                }
                else {
                    throwSyntaxError("question mark", currentToken.getValue(), currentToken);
                }
            }
            else {
                if (auto ternary = tryBuildTernaryExpression(subExpr); ternary) {
                    return ternary;
                }
                else {
                    throwSyntaxError("question mark", currentToken.getValue(), currentToken);
                }
            }
        }
        else {
            return expr;
        }
    } else
        return std::nullopt;
}

std::optional<Expression> Parser::tryBuildSimpleExpression() {
    if (auto brackets = tryBuildBrackets(); brackets) {
        return tryBuildMultiplicationOrAddition(*brackets);
    }
    else if (auto val = tryBuildValue(); val) {
        if (auto addOrMult = tryBuildMultiplicationOrAddition(*val); addOrMult) {
            return addOrMult;
        }
        else {
            return val;
        }
    }
    else {
        return std::nullopt;
    }
}

std::optional<Expression> Parser::tryBuildMultiplicationOrAddition(Expression& expr) {
    if (auto multi = tryBuildMultiplication(expr); multi) {
        Expression multExpr = std::move(multi);
        if (auto add = tryBuildAddition(multExpr); add) {
            return add;
        }
        return multExpr;
    } else if (auto add = tryBuildAddition(expr); add) {
        return add;
    }
    else {
        return std::nullopt;
    }
}

TernaryExpressionPtr Parser::tryBuildTernaryExpression(LogicalSubExpressionPtr &condition) {
    if (currentToken.getType() != Token::TokenType::QUESTION_MARK) return nullptr;
    getNextToken();
    if (auto ifTrue = tryBuildExpression(); ifTrue) {
        if (currentToken.getType() != Token::TokenType::COLON)
            throwSyntaxError("colon", currentToken.getValue(), currentToken);
        getNextToken();
        if (auto ifFalse = tryBuildExpression(); ifFalse) {
            return std::make_unique<TernaryExpression>(getLogicalSubExpressionPosition(*condition), condition, *ifTrue, *ifFalse);
        }
        throwSyntaxError("expression", currentToken.getValue(), currentToken);
    }
    throwSyntaxError("expression", currentToken.getValue(), currentToken);
}

MultiplicationPtr Parser::tryBuildMultiplication(Expression& firstValue) {
    if (!MultiplicationFactory::isMultiplicationOperator(currentToken.getType()))
        return nullptr;
    auto operatorToken = currentToken;
    getNextToken();
    if (auto secondValue = tryBuildExpression(); secondValue) {
        return MultiplicationFactory::getMultiplication(getExpressionPosition(firstValue),
            firstValue, *secondValue, operatorToken.getType());
    }
    throwSyntaxError("value after " + operatorToken.getValue(), currentToken.getValue(), currentToken);
}

AdditionPtr Parser::tryBuildAddition( Expression& firstValue) {
    if(!AdditionFactory::isAdditionOperator(currentToken.getType()))
        return nullptr;
    auto operatorToken = currentToken;
    getNextToken();
    if (auto secondValue = tryBuildExpression(); secondValue) {
         return AdditionFactory::getAddition(getExpressionPosition(firstValue),
            firstValue, *secondValue, operatorToken.getType());
    }
    throwSyntaxError("value after " + operatorToken.getValue(), currentToken.getValue(), currentToken);
}

LogicalSubExpressionPtr Parser::tryBuildLogicalExpressionInBrackets() {
    if (currentToken.getType() == Token::TokenType::OPENING_BRACKET) {
        auto openingToken = currentToken;
        getNextToken();
        if (auto expr = tryBuildSimpleExpression(); expr) {
            if (auto comparison = tryBuildComparison(*expr); comparison) {
                auto subExpr = std::make_unique<LogicalSubExpression>(std::move(comparison));
                if (auto subExpr1 = tryBuildLogicalExpression(subExpr); subExpr1) {
                    if (currentToken.getType() == Token::TokenType::CLOSING_BRACKET) {
                        getNextToken();
                        return std::make_unique<LogicalSubExpression>(std::move(subExpr1));
                    }
                    throwSyntaxError("closing bracket", currentToken.getValue(), currentToken);
                }
                if (currentToken.getType() == Token::TokenType::CLOSING_BRACKET) {
                    getNextToken();
                    return subExpr;
                }
                throwSyntaxError("closing bracket", currentToken.getValue(), currentToken);
            }        
            if (currentToken.getType() == Token::TokenType::CLOSING_BRACKET) {
                getNextToken();
                if (auto add = tryBuildMultiplicationOrAddition(*expr); add) {
                    if (auto comparison = tryBuildComparison(*add); comparison) {
                        auto subExpr = std::make_unique<LogicalSubExpression>(std::move(comparison));
                        if (auto subExpr1 = tryBuildLogicalExpression(subExpr); subExpr1) {
                            return std::make_unique<LogicalSubExpression>(std::move(subExpr1));
                        }
                        return subExpr;
                    }
                    throwSyntaxError("logical expression or comparison", "expression", openingToken);
                }
                throwSyntaxError("logical expression or comparison", "expression", openingToken);
            }
            throwSyntaxError("expression", "empty brackets", openingToken);
        }
        throwSyntaxError("logical expression or comparison", "empty brackets", openingToken);
    }
    return nullptr;
}


PropertyPtr Parser::tryBuildProperty(Identifier& ident) {
    if (ident.hasNext()) return nullptr;
    if(currentToken.getType() != Token::TokenType::COLON) return nullptr;
    getNextToken();
    if (auto expr = tryBuildExpression(); expr) {
        return std::make_unique<Property>(ident.getPosition(), ident, *expr);
    }
    throwSyntaxError("expression", currentToken.getValue(), currentToken);
}


BasicObjectPtr Parser::tryBuildBasicObject() {
    if (!BasicObjectFactory::isBasicObjectNameToken(currentToken)) return nullptr;
    auto startingToken = currentToken;
    Properties properties;
    getNextToken();
    if(currentToken.getType() != Token::TokenType::OPENING_BRACE) throwSyntaxError("{", currentToken.getValue(), currentToken);
    getNextToken(); 
    while(auto ident = tryBuildIdentifier()) {
        if (auto property = tryBuildProperty(*ident); property) {
            properties.push_back(std::move(property));
        }
        else {
            throwSyntaxError("expression", currentToken.getValue(), currentToken);
        }
    }
    if (currentToken.getType() != Token::TokenType::CLOSING_BRACE) throwSyntaxError("}", currentToken.getValue(), currentToken);
    getNextToken();
    return BasicObjectFactory::getBasicObject(startingToken.getPosition(), properties, startingToken);
}

ComplexObjectPtr Parser::tryBuildComplexObject()
{
    if (BasicObjectFactory::isBasicObjectNameToken(currentToken)) return nullptr;
    if (!isKnownType(currentToken.getValue())) return nullptr;
    auto startingToken = currentToken;
    Properties properties;
    Objects objects;
    getNextToken();
    if (currentToken.getType() != Token::TokenType::OPENING_BRACE) throwSyntaxError("{", currentToken.getValue(), currentToken);
    getNextToken();
    while (auto ident = tryBuildIdentifier()) {
        if (auto property = tryBuildProperty(*ident); property) {
            properties.push_back(std::move(property));
        }
        else {
            if (currentToken.getType() != TokenType::EQUAL_SIGN) throwSyntaxError("= or :", currentToken.getValue(), currentToken);
            getNextToken();
            IdentifierPtr identPtr = std::make_unique<Identifier>(std::move(*ident));
            if (auto obj = tryBuildBasicObject()) {
                objects.push_back(std::make_unique<ObjectIdentifierPair>(*ident, obj));
            }
            else if (auto obj = tryBuildComplexObject()) {
                objects.push_back(std::make_unique<ObjectIdentifierPair>(*ident, obj));
            }
            else {
                throwSyntaxError("object declaration", currentToken.getValue(), currentToken);
            }
        }
    }
    if (currentToken.getType() != Token::TokenType::CLOSING_BRACE) throwSyntaxError("}", currentToken.getValue(), currentToken);
    getNextToken();
    return std::make_unique<ComplexObject>(startingToken.getValue(), startingToken.getPosition(), properties, objects);
}

LogicalExpressionPtr Parser::tryBuildLogicalExpression(LogicalSubExpressionPtr &firstValue) {
    if (!firstValue) return nullptr;
    if (!LogicalExpressionFactory::isLogicalOperator(currentToken.getType())) return nullptr;
    auto operatorToken = currentToken;
    getNextToken();
    if (auto subExpression = tryBuildLogicalExpressionInBrackets(); subExpression) {
        return getLogicalSubExpression(operatorToken, subExpression, firstValue);
    }
    if (auto secondValue = tryBuildSimpleExpression(); secondValue) {
        if (auto comparison = tryBuildComparison(*secondValue); comparison) {
            LogicalSubExpressionPtr subExpr(std::make_unique<LogicalSubExpression>(std::move(comparison)));
            return getLogicalSubExpression(operatorToken, subExpr, firstValue);
        }
    }
    throwSyntaxError("value", currentToken.getValue(), operatorToken);
}
LogicalExpressionPtr Parser::getLogicalSubExpression(const Token& operatorToken, LogicalSubExpressionPtr& comparison, LogicalSubExpressionPtr& firstValue){
    if (operatorToken.getType() == Token::TokenType::OR) {//DisjunctionExpression
        if (auto nextExpr = tryBuildLogicalExpression(comparison); nextExpr) {
            auto secondValue = std::make_unique<LogicalSubExpression>(std::move(nextExpr));
            return std::make_unique<DisjunctionExpression>(getLogicalSubExpressionPosition(*firstValue.get()),
                firstValue, secondValue);
        }
        return std::make_unique<DisjunctionExpression>(getLogicalSubExpressionPosition(*firstValue.get()),
            firstValue, comparison);
    }
    //Token::TokenType::AND ConjuctionExpression
    LogicalSubExpressionPtr expression(std::make_unique<LogicalSubExpression>(
        std::make_unique<ConjuctionExpression>(getLogicalSubExpressionPosition(*firstValue.get()),
        firstValue, comparison)));
    if (auto nextExpr = tryBuildLogicalExpression(expression); nextExpr)
        return nextExpr;
    return std::move(std::get<LogicalExpressionPtr>(*(expression.get())));
}

std::unique_ptr<Comparison> Parser::tryBuildComparison(Expression& firstValue) {
    auto comparisonToken = currentToken;
    if(!ComparisonFactory::isComparisonOperator(currentToken.getType()))
        return nullptr;
    auto nextToken = getNextToken();
    if (auto secondValue = tryBuildExpression(); secondValue) {
        return ComparisonFactory::getComparison(getExpressionPosition(firstValue), firstValue, 
            *secondValue, comparisonToken.getType());
    }
    throwSyntaxError("value after " + comparisonToken.getValue(), nextToken.getValue(), nextToken);
}


std::optional<Scene> Parser::tryBuildScene() { //TODO
    return std::nullopt;
}

std::unique_ptr<SceneRoot> Parser::parse() {
    try {
        while (currentToken.getType() != TokenType::END_OF_FILE) {
            if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) throwSyntaxError("expected animation, complex object, or scene declaration", currentToken.getValue(), currentToken);
            auto ident = currentToken;
            if (auto scene = tryBuildScene(); scene) {
                //TODO
            }
            else {
                if(getNextToken().getType() != TokenType::EQUAL_SIGN)
                    throwSyntaxError("expected animation, complex object, or scene declaration", currentToken.getValue(), currentToken);
                getNextToken();
                if (auto anim = tryBuildAnimationDeclaration(); anim) {
                    //add animations to list of animations TODO
                }else if (auto obj = tryBuildNewComplexObject(ident.getValue()); obj) {
                    if (isKnownType(ident.getValue())) throw SyntaxError("Redefinition of " + ident.getValue() +
                        std::string(" ", 1) + ident.getPosition().toString() + scanner.getLineError(ident.getPosition()));
                    knownTypes.push_back(std::move(obj));
                }
                else throwSyntaxError("expected animation, complex object, or scene declaration", currentToken.getValue(), currentToken);
            }
            try {
                scanner.next();
            } catch (const SyntaxError& error) {
                std::cout << error.what() << '\n';
                return nullptr;
            }
        }
    }
    catch (SyntaxError er) {
        std::cout << er.what();
    }
    catch (...) {
        std::cout << "Unknown exception" << std::endl;
        throw;
    }
    std::cout << std::endl;
	return std::move(root);
}