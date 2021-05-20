#include "Parser.h"
#include "../Exceptions/SyntaxError.h"
#include "../Exceptions/Redefinition.h"
#include "../Exceptions/NotDefined.h"
#include <optional>
#include <functional>

Parser::Parser(Scanner& scanner) : scanner(scanner) {
    currentToken = scanner.getToken();
}

const Token& Parser::getNextToken() {
    scanner.next();
    currentToken = scanner.getToken();
    return currentToken;
}


bool Parser::isNotCurrentToken(TokenType expected) noexcept {
    if (currentToken.getType() != expected)
        return true;
    getNextToken();
    return false;
}

void Parser::consumeToken(TokenType expected) {
    if (currentToken.getType() == expected)
        getNextToken();
    else
        throw SyntaxError(Token::TokenTypeToStringMap.at(expected), currentToken);
}

bool Parser::tryBuildObjectOrProperty(Properties& properties, Objects& objects) {
    auto ident = tryBuildIdentifier();
    if (!ident) return false;
    if (auto property = tryBuildProperty(*ident); property) {
        properties.push_back(std::move(property));
        return true;
    }
    consumeToken(TokenType::EQUAL_SIGN);
    if (auto obj = tryBuildBasicObject()) {
        objects.push_back(std::make_unique<ObjectIdentifierPair>(*ident, obj));
        return true;
    }
    if (auto obj = tryBuildComplexObject()) {
        objects.push_back(std::make_unique<ObjectIdentifierPair>(*ident, obj));
        return true;
    }
    throw SyntaxError("object declaration", currentToken);
}

ComplexObjectDeclarationPtr Parser::tryBuildNewComplexObject(const Token& identifierToken) {//'Identifier =' should already be found by more general function, this only checks what's after
    if (identifierToken.getType() != TokenType::TYPE_IDENTIFIER) return nullptr;
    if (isNotCurrentToken(TokenType::OPENING_BRACE)) return nullptr;
    Objects objects;
    Properties properties;
    while (tryBuildObjectOrProperty(properties, objects));
    if (currentToken.getType() != TokenType::CLOSING_BRACE) throw SyntaxError("closing bracket or identifier", currentToken);
    getNextToken();
    return std::make_unique<ComplexObjectDeclaration>(identifierToken.getValue(), identifierToken.getPosition(), properties, objects);
}

bool Parser::isKnownType(const std::string& value) {
    return std::find_if(knownTypes.begin(), knownTypes.end(), [&value](const auto& val) {
        return value == val->getName();
     }) != knownTypes.end();
}

bool Parser::isKnownAnimation(const std::string& value) {
    return std::find_if(knownAnimations.begin(), knownAnimations.end(), [&value](const auto& val) {
        return value == val->getName();
        }) != knownAnimations.end();
} 

bool Parser::tryBuildAnimationOrProperty(Properties& properties, Animations& animations) {
    if (auto animation = tryBuildAnimation(); animation) {
        animations.push_back(std::move(animation));
        return true;
    }
    Identifier ident(currentToken.getPosition(), currentToken.getValue());
    if (isNotCurrentToken(TokenType::VARIABLE_IDENTIFIER)) return false;
    if (auto property = tryBuildProperty(ident); property) {
        properties.push_back(std::move(property));
        return true;
    }
    throw SyntaxError("property declaration", currentToken);
}

AnimationDeclarationPtr Parser::tryBuildAnimationDeclaration(const Token& identifierToken) {
    if (identifierToken.getType() != TokenType::TYPE_IDENTIFIER) return nullptr;
    if (isNotCurrentToken(TokenType::ANIMATION_SEQUENCE_KEYWORD)) return nullptr;
    consumeToken(TokenType::OPENING_BRACKET);
    std::vector<Identifier> args;
    do {
        args.push_back(Identifier(currentToken.getPosition(), currentToken.getValue()));
        consumeToken(TokenType::VARIABLE_IDENTIFIER);
    } while (!isNotCurrentToken(TokenType::COMMA));
    consumeToken(TokenType::CLOSING_BRACKET);
    consumeToken(TokenType::OPENING_BRACE);
    Properties properties;
    Animations animations;
    while (tryBuildAnimationOrProperty(properties, animations));
    consumeToken(TokenType::CLOSING_BRACE);
    return std::make_unique<AnimationDeclaration>(identifierToken.getPosition(), identifierToken.getValue(), args, properties, animations);
}

bool Parser::tryBuildProperty(Properties& properties) {
    auto ident = tryBuildIdentifier();
    if (!ident) return false;
    if (auto property = tryBuildProperty(*ident); property) {
        properties.push_back(std::move(property));
        return true;
    }
    return false;
}

AnimationPtr Parser::tryBuildWait() {
    if(isNotCurrentToken(TokenType::WAIT_KEYWORD))return nullptr;
    auto startingToken = currentToken;
    consumeToken(TokenType::OPENING_BRACE);
    Properties properties;
    while (tryBuildProperty(properties));
    consumeToken(TokenType::CLOSING_BRACE);
    return std::make_unique<Wait>(startingToken.getPosition(), properties);
}


AnimationPtr Parser::tryBuildBasicAnimation() {
    auto startingToken = currentToken;
    if (isNotCurrentToken(TokenType::ANIMATION_KEYWORD)) return nullptr;
    consumeToken(TokenType::OPENING_BRACE);
    Properties properties;
    while (tryBuildProperty(properties));
    consumeToken(TokenType::CLOSING_BRACE);
    return std::make_unique<Animation>(startingToken.getPosition(), properties);
}

AnimationPtr Parser::tryBuildComplexAnimation() {
    if (!(currentToken.getType() == TokenType::PARALEL_ANIMATION_KEYWORD 
        || currentToken.getType() == TokenType::ANIMATION_SEQUENCE_KEYWORD)) return nullptr;
    auto startingToken = currentToken;
    getNextToken();
    consumeToken(TokenType::OPENING_BRACE);
    Properties properties;
    Animations animations;
    while (tryBuildAnimationOrProperty(properties, animations));
    consumeToken(TokenType::CLOSING_BRACE);
    if(startingToken.getType() == TokenType::PARALEL_ANIMATION_KEYWORD)
        return std::make_unique<ParalelAnimation>(startingToken.getPosition(), properties, animations);
    return std::make_unique<AnimationSequence>(startingToken.getPosition(), properties, animations);
}

LogicalSubExpressionPtr Parser::tryBuildCondition() {
    Identifier ident(currentToken.getPosition(), currentToken.getValue());
    if (isNotCurrentToken(TokenType::CONDITION_KEYWORD)) return nullptr;
    consumeToken(TokenType::COLON);
    if (auto inBrackets = tryBuildLogicalExpressionInBrackets(); inBrackets) {
        if (auto subExpr1 = tryBuildLogicalExpression(inBrackets); subExpr1)
            return std::make_unique<LogicalSubExpression>(std::move(subExpr1));
        else
            return std::move(inBrackets);
    }
    auto exp = tryBuildValue();
    if (!exp) throw SyntaxError("logical expression", currentToken);
    if (auto comparison = tryBuildComparison(*exp); comparison) {
        auto subExpr = std::make_unique<LogicalSubExpression>(std::move(comparison));
        if (auto subExpr1 = tryBuildLogicalExpression(subExpr); subExpr1)
            return std::make_unique<LogicalSubExpression>(std::move(subExpr1));
        else
            return std::move(subExpr);
    }
    throw SyntaxError("logical expression", currentToken);
}

bool Parser::tryBuildAnimationPropertyOrCondition(Animations& animations, Properties& properties, LogicalSubExpressionPtr& condition) {
    if (auto animation = tryBuildAnimation(); animation) {
        animations.push_back(std::move(animation));
        return true;
    }
    if (currentToken.getType() == TokenType::VARIABLE_IDENTIFIER) {
        Identifier ident(currentToken.getPosition(), currentToken.getValue());
        getNextToken();
        if (auto property = tryBuildProperty(ident); property) {
            properties.push_back(std::move(property));
            return true;
        }
        else throw SyntaxError("property", currentToken);
    }
    if (auto cond = tryBuildCondition(); cond) {
        if (condition) throw Redefinition("condition property", currentToken.getPosition());
        condition = std::move(cond);
        return true;
    }
    return false;
}

AnimationPtr Parser::tryBuildConditionalAnimation() {
    auto startingToken = currentToken;
    if(isNotCurrentToken(TokenType::CONDITIONAL_ANIMATION_KEYWORD)) return nullptr;
    consumeToken(TokenType::OPENING_BRACE);
    Properties properties;
    LogicalSubExpressionPtr condition;
    Animations animations;  
    while (tryBuildAnimationPropertyOrCondition(animations, properties, condition));
    consumeToken(TokenType::CLOSING_BRACE);
    return std::make_unique<ConditionalAnimation>(startingToken.getPosition(), properties, animations, condition);
}

AnimationPtr Parser::tryBuildAnimation() {
    if (auto wait = tryBuildWait(); wait)
        return std::move(wait);
    //paralel or sequence
    if (auto anim = tryBuildComplexAnimation(); anim)
        return std::move(anim);
    if (auto cond = tryBuildConditionalAnimation(); cond)
        return std::move(cond);
    if (auto basic = tryBuildBasicAnimation(); basic)
        return std::move(basic);
    return nullptr;
}

std::optional<Color> Parser::tryBuildColor() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER || currentToken.getValue() != "Color") 
        return std::nullopt;
    Position startingColorPosition = currentToken.getPosition();
    if (getNextToken().getType() != TokenType::OPENING_BRACKET) 
        throw SyntaxError("'('", currentToken);
    if (getNextToken().getType() != TokenType::HEX_CONST) 
        throw SyntaxError("hexadecimal value", currentToken);
    auto firstHexValue = currentToken.getValue();
    if (getNextToken().getType() != TokenType::COMMA) 
        throw SyntaxError("','", currentToken);
    if (getNextToken().getType() != TokenType::HEX_CONST) 
        throw SyntaxError("hexadecimal value", currentToken);
    auto secondHexValue = currentToken.getValue();
    if (getNextToken().getType() != TokenType::COMMA) 
        throw SyntaxError("','", currentToken);
    if (getNextToken().getType() != TokenType::HEX_CONST) 
        throw SyntaxError("hexadecimal value", currentToken);
    auto thirdHexValue = currentToken.getValue();
    if (getNextToken().getType() != TokenType::CLOSING_BRACKET) 
        throw SyntaxError("')'", currentToken);
    getNextToken();
    return Color(startingColorPosition, firstHexValue, secondHexValue, thirdHexValue);
}

std::optional<DecimalValue> Parser::tryBuildDecimalValue() {
    Position pos = currentToken.getPosition();
    if (currentToken.getType() == TokenType::MINUS) {
        if (getNextToken().getType() != TokenType::DECIMAL_CONST) 
            throw SyntaxError("decimal value", currentToken);
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

std::optional<Identifier> Parser::tryBuildIdentifier() {
    if (currentToken.getType() != TokenType::VARIABLE_IDENTIFIER)
        return std::nullopt;

    std::vector<std::pair<Position, std::string>> identifiers;
    identifiers.push_back(std::make_pair(currentToken.getPosition(), currentToken.getValue()));
    while (getNextToken().getType() == TokenType::DOT) {
        if (getNextToken().getType() != TokenType::VARIABLE_IDENTIFIER)
            throw SyntaxError("variable identifier after '.'", currentToken);
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

std::optional<Point> Parser::tryBuildPoint() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER || currentToken.getValue() != "Point") 
        return std::nullopt;
    Position startingPointPosition = currentToken.getPosition();
    if (getNextToken().getType() != TokenType::OPENING_BRACKET)
        throw SyntaxError("'('", currentToken);
    if (getNextToken().getType() == TokenType::CLOSING_BRACKET) {
        getNextToken();
        return Point(startingPointPosition);
    }
    auto firstDecimalValue = tryBuildDecimalValue();
    if (!firstDecimalValue) 
        throw SyntaxError("')' or decimal value", currentToken);
    if (isNotCurrentToken(TokenType::COMMA)) {
        consumeToken(TokenType::CLOSING_BRACKET);
        return Point(startingPointPosition, *firstDecimalValue);
    }
    auto secondDecimalValue = tryBuildDecimalValue();
    if (!secondDecimalValue)
        throw SyntaxError("decimal value", currentToken);
    if (isNotCurrentToken(TokenType::COMMA)) {
        consumeToken(TokenType::CLOSING_BRACKET);
        return Point(startingPointPosition, *firstDecimalValue, *secondDecimalValue);
    }
    auto thirdDecimalValue = tryBuildDecimalValue();
    if (!thirdDecimalValue) throw SyntaxError("decimal value", currentToken);
    consumeToken(TokenType::CLOSING_BRACKET);
    return Point(startingPointPosition, *firstDecimalValue, *secondDecimalValue, *thirdDecimalValue);
}

std::optional<TimeDeclaration> Parser::tryBuildTimeDeclaration() {
    auto pos = currentToken.getPosition();
    if (isNotCurrentToken(TokenType::DOUBLE_QUOTE_CHARACTER)) return std::nullopt;
    auto decValue = currentToken.getValue();
    consumeToken(TokenType::DECIMAL_CONST);  
    auto timeSpecifier = currentToken.getValue();
    consumeToken(TokenType::VARIABLE_IDENTIFIER);
    if(!TimeDeclaration::isCorrectTimeSpecifier(timeSpecifier)) throw SyntaxError("time specifier", currentToken);
    consumeToken(TokenType::DOUBLE_QUOTE_CHARACTER);
    return TimeDeclaration(pos, decValue, timeSpecifier);
}

std::optional<ConstantIdentifier> Parser::tryBuildConstantIdentifier() {
    std::vector<std::pair<Position, std::string>> identifiers;
    identifiers.push_back(std::make_pair(currentToken.getPosition(), currentToken.getValue()));
    if (isNotCurrentToken(TokenType::TYPE_IDENTIFIER)) return std::nullopt;
    while (!isNotCurrentToken(TokenType::DOT)) {
        identifiers.push_back(std::make_pair(currentToken.getPosition(), currentToken.getValue()));
        consumeToken(TokenType::TYPE_IDENTIFIER);
    }
    ConstantIdentifierPtr identifier_ptr;
    while (identifiers.size() > 1) {
        identifier_ptr = std::make_unique<ConstantIdentifier>(identifiers.back().first, identifiers.back().second, identifier_ptr);
        identifiers.pop_back();
    }
    return ConstantIdentifier(identifiers[0].first, identifiers[0].second, identifier_ptr);
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
    if (auto time = tryBuildTimeDeclaration(); time) {
        return time;
    }
    if (auto constIdentifier = tryBuildConstantIdentifier(); constIdentifier) {
        return constIdentifier;
    }
    if (auto arr = tryBuildArrayType(); arr) {
        return arr;
    }
    return std::nullopt;
}

std::optional<PointArray> Parser::tryBuildPointArray(const Position& pos) { //opening square brace checked by calling function
    std::vector<Point> points;
    auto firstPoint = tryBuildPoint();
    if (!firstPoint) return std::nullopt;
    while (!isNotCurrentToken(TokenType::COMMA)) {
        auto point = tryBuildPoint();
        if(!point) throw SyntaxError("vertex declaration", currentToken);
    }
    consumeToken(TokenType::CLOSING_SQUARE_BRACE);
    return PointArray(pos, points);
}


Argument Parser::tryBuildArgument() {
    Argument arg;
    if (isNotCurrentToken(TokenType::OPENING_SQUARE_BRACE)) return arg;
    auto expr = tryBuildExpression();
    while (!isNotCurrentToken(TokenType::COMMA)) {
        if (auto expr = tryBuildExpression(); expr) {
            arg.push_back(std::move(*expr));
        }
        else throw SyntaxError("expression", currentToken);
    }
    consumeToken(TokenType::CLOSING_SQUARE_BRACE);
    return std::move(arg);
}

Arguments Parser::tryBuildArguments() {
    Arguments args;
    do {
        if(!args.empty()) getNextToken();
        if (auto arg = tryBuildArgument(); !arg.empty()) {
            args.push_back(std::move(arg));
        }
        else if (auto expr = tryBuildExpression(); expr) {
            Argument arg;
            arg.push_back(std::move(*expr));
            args.push_back(std::move(arg));
        }
        else if (!args.empty()) throw SyntaxError("expression", currentToken);
    } while (currentToken.getType() == TokenType::COMMA);
    if (currentToken.getType() != TokenType::CLOSING_BRACKET) throw SyntaxError("closing bracket", currentToken);
    getNextToken();
    return std::move(args);
}

AnimationCallPtr Parser::tryBuildAnimationCall() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) return nullptr;
    auto startToken = currentToken;
    if (getNextToken().getType() != TokenType::OPENING_BRACKET) throw SyntaxError("opening bracket", currentToken);
    getNextToken();
    Arguments args = tryBuildArguments();
    return std::make_unique<AnimationCall>(startToken.getPosition(), startToken.getValue(), args);
}

std::optional<AnimationProperty> Parser::tryBuildAnimationProperty(const Position& pos) {
    std::vector<AnimationCallPtr> animationCalls;
    do {
        if (!animationCalls.empty()) getNextToken();
        if (auto anim = tryBuildAnimationCall(); anim)
            animationCalls.push_back(std::move(anim));
        else {
            if (animationCalls.empty()) return std::nullopt;
            throw SyntaxError("animation call declaration", currentToken);
        }
    } while (currentToken.getType() == TokenType::COMMA);

    if (currentToken.getType() != TokenType::CLOSING_SQUARE_BRACE) throw SyntaxError("closing square brace", currentToken);
    getNextToken();
    return AnimationProperty(pos, animationCalls);
}

std::optional<Expression> Parser::tryBuildArrayType() {
    if (currentToken.getType() != TokenType::OPENING_SQUARE_BRACE) return std::nullopt;
    const Position& pos = currentToken.getPosition();
    getNextToken();
    if (currentToken.getType() == TokenType::CLOSING_SQUARE_BRACE) throw SyntaxError("array content", currentToken);
    if (auto points = tryBuildPointArray(pos); points) {
        return points;
    }
    if (auto anim = tryBuildAnimationProperty(pos); anim) {
        return anim;
    }
    throw SyntaxError("array", currentToken);
}

std::optional<Expression> Parser::tryBuildBrackets() {
    if (currentToken.getType() == TokenType::OPENING_BRACKET) {
        auto openingToken = currentToken;
        getNextToken();
        if (auto subExpr = tryBuildExpression(); subExpr) {
            if (currentToken.getType() == TokenType::CLOSING_BRACKET) {
                getNextToken();
                return subExpr;
            }
            throw SyntaxError("expression", "empty brackets", openingToken.getPosition());
        }
        throw SyntaxError("closing bracket", currentToken.getValue(), openingToken.getPosition());
    }
    return std::nullopt;
}

std::optional<Expression> Parser::tryBuildExpression() {
    if (auto expr = tryBuildSimpleExpression(); expr) {
        if (auto comparison = tryBuildComparison(*expr); comparison) {
            auto subExpr = std::make_unique<LogicalSubExpression>(std::move(comparison));
            if (auto logicalExpr =  tryBuildLogicalExpression(subExpr); logicalExpr) {
                LogicalSubExpressionPtr logicSubExpr = std::make_unique<LogicalSubExpression>(std::move(logicalExpr));
                if (auto ternary = tryBuildTernaryExpression(logicSubExpr); ternary)
                    return ternary;
                throw SyntaxError("question mark", currentToken);
            }
            if (auto ternary = tryBuildTernaryExpression(subExpr); ternary)
                return ternary;
            throw SyntaxError("question mark", currentToken);
        }
        return expr;
    }
    return std::nullopt;
}

std::optional<Expression> Parser::tryBuildSimpleExpression() {
    if (auto brackets = tryBuildBrackets(); brackets) {
        if (auto multOrAdd = tryBuildMultiplicationOrAddition(*brackets); multOrAdd) {
            return multOrAdd;
        }
        return brackets;
    }
    else if (auto val = tryBuildValue(); val) {
        if (auto addOrMult = tryBuildMultiplicationOrAddition(*val); addOrMult) {
            return addOrMult;
        }
        else return val;
    }
    return std::nullopt;
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
    if (currentToken.getType() != TokenType::QUESTION_MARK) return nullptr;
    getNextToken();
    if (auto ifTrue = tryBuildExpression(); ifTrue) {
        if (currentToken.getType() != TokenType::COLON)
            throw SyntaxError("colon", currentToken);
        getNextToken();
        if (auto ifFalse = tryBuildExpression(); ifFalse) {
            return std::make_unique<TernaryExpression>(getLogicalSubExpressionPosition(*condition), condition, *ifTrue, *ifFalse);
        }
        throw SyntaxError("expression", currentToken);
    }
    throw SyntaxError("expression", currentToken);
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
    throw SyntaxError("value after " + operatorToken.getValue(), currentToken);
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
    throw SyntaxError("value after " + operatorToken.getValue(), currentToken);
}

LogicalSubExpressionPtr Parser::tryBuildLogicalExpressionInBrackets() {
    if (currentToken.getType() == TokenType::OPENING_BRACKET) {
        auto openingToken = currentToken;
        getNextToken();
        if (auto expr = tryBuildSimpleExpression(); expr) {
            if (auto comparison = tryBuildComparison(*expr); comparison) {
                auto subExpr = std::make_unique<LogicalSubExpression>(std::move(comparison));
                if (auto subExpr1 = tryBuildLogicalExpression(subExpr); subExpr1) {
                    if (currentToken.getType() == TokenType::CLOSING_BRACKET) {
                        getNextToken();
                        return std::make_unique<LogicalSubExpression>(std::move(subExpr1));
                    }
                    throw SyntaxError("closing bracket", currentToken);
                }
                if (currentToken.getType() == TokenType::CLOSING_BRACKET) {
                    getNextToken();
                    return subExpr;
                }
                throw SyntaxError("closing bracket", currentToken);
            }        
            if (currentToken.getType() == TokenType::CLOSING_BRACKET) {
                getNextToken();
                if (auto add = tryBuildMultiplicationOrAddition(*expr); add) {
                    if (auto comparison = tryBuildComparison(*add); comparison) {
                        auto subExpr = std::make_unique<LogicalSubExpression>(std::move(comparison));
                        if (auto subExpr1 = tryBuildLogicalExpression(subExpr); subExpr1) {
                            return std::make_unique<LogicalSubExpression>(std::move(subExpr1));
                        }
                        return subExpr;
                    }
                    throw SyntaxError("logical expression or comparison", "expression", openingToken.getPosition());
                }
                throw SyntaxError("logical expression or comparison", "expression", openingToken.getPosition());
            }
            throw SyntaxError("expression", "empty brackets", openingToken.getPosition());
        }
        throw SyntaxError("logical expression or comparison", "empty brackets", openingToken.getPosition());
    }
    return nullptr;
}

PropertyPtr Parser::tryBuildProperty(Identifier& ident) {
    if (ident.hasNext()) return nullptr;
    if(isNotCurrentToken(TokenType::COLON)) return nullptr;
    if (auto expr = tryBuildExpression(); expr) {
        return std::make_unique<Property>(ident.getPosition(), ident, *expr);
    }
    throw SyntaxError("expression", currentToken);
}


BasicObjectPtr Parser::tryBuildBasicObject() {
    if (!BasicObjectFactory::isBasicObjectNameToken(currentToken)) return nullptr;
    auto startingToken = currentToken;
    Properties properties;
    getNextToken();
    consumeToken(TokenType::OPENING_BRACE);
    while(auto ident = tryBuildIdentifier()) {
        if (auto property = tryBuildProperty(*ident); property) {
            properties.push_back(std::move(property));
        }
        else throw SyntaxError("expression", currentToken);
    }
    consumeToken(TokenType::CLOSING_BRACE);
    return BasicObjectFactory::getBasicObject(startingToken.getPosition(), properties, startingToken);
}

ComplexObjectPtr Parser::tryBuildComplexObject() {
    if (BasicObjectFactory::isBasicObjectNameToken(currentToken)) return nullptr;
    auto startingToken = currentToken;
    Properties properties;
    if(isNotCurrentToken(TokenType::TYPE_IDENTIFIER)) return nullptr;
    consumeToken(TokenType::OPENING_BRACE);
    while (auto ident = tryBuildIdentifier()) {
        if (auto property = tryBuildProperty(*ident); property) {
            properties.push_back(std::move(property));
        }
        else
            throw SyntaxError("property declaration", currentToken);
    }
    consumeToken(TokenType::CLOSING_BRACE);
    return std::make_unique<ComplexObject>(startingToken.getValue(), startingToken.getPosition(), properties);
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
    throw SyntaxError("expression", currentToken);
}
LogicalExpressionPtr Parser::getLogicalSubExpression(const Token& operatorToken, LogicalSubExpressionPtr& comparison, LogicalSubExpressionPtr& firstValue){
    if (operatorToken.getType() == TokenType::OR) {//DisjunctionExpression
        if (auto nextExpr = tryBuildLogicalExpression(comparison); nextExpr) {
            auto secondValue = std::make_unique<LogicalSubExpression>(std::move(nextExpr));
            return std::make_unique<DisjunctionExpression>(getLogicalSubExpressionPosition(*firstValue.get()),
                firstValue, secondValue);
        }
        return std::make_unique<DisjunctionExpression>(getLogicalSubExpressionPosition(*firstValue.get()),
            firstValue, comparison);
    }
    //TokenType::AND ConjuctionExpression
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
    throw SyntaxError("value after " + comparisonToken.getValue(), nextToken);
}


ScenePtr Parser::tryBuildScene() {
    auto pos = currentToken.getPosition();
    if (isNotCurrentToken(TokenType::SCENE_KEYWORD)) return nullptr;
    consumeToken(TokenType::OPENING_BRACE);
    Objects objects;
    Properties properties;
    while (auto ident = tryBuildIdentifier()) {
        if (auto property = tryBuildProperty(*ident); property) {
            properties.push_back(std::move(property));
        }
        else {
            consumeToken(TokenType::EQUAL_SIGN);
            if (auto obj = tryBuildBasicObject(); obj) {
                objects.push_back(std::make_unique<ObjectIdentifierPair>(*ident, obj));
            }
            else if (auto obj = tryBuildComplexObject(); obj) {
                objects.push_back(std::make_unique<ObjectIdentifierPair>(*ident, obj));
            } else throw SyntaxError("object declaration", currentToken);
        }
    }
    consumeToken(TokenType::CLOSING_BRACE);
    return std::make_unique<Scene>(pos, objects, properties);
}

bool Parser::tryBuildMainObject() {
    if (auto scene = tryBuildScene(); scene) {
        this->scene = std::move(scene);
        return true;
    }
    auto ident = currentToken;
    if (isNotCurrentToken(TokenType::TYPE_IDENTIFIER)) return false;
    consumeToken(TokenType::EQUAL_SIGN);
    if (auto anim = tryBuildAnimationDeclaration(ident); anim) {
        if (isKnownAnimation(ident.getValue())) throw Redefinition(ident.getValue(), ident.getPosition());
        knownAnimations.push_back(std::move(anim));
        return true;
    }
    if (auto obj = tryBuildNewComplexObject(ident); obj) {
        if (isKnownType(ident.getValue())) throw Redefinition(ident.getValue(), ident.getPosition());
        knownTypes.push_back(std::move(obj));
        return true;
    }
    return false;
}

std::unique_ptr<SceneRoot> Parser::parse() {
    while (tryBuildMainObject());
    if(currentToken.getType() != TokenType::END_OF_FILE)
        throw SyntaxError("animation, complex object, or scene declaration", currentToken);
    if (!scene)
        throw NotDefined("Scene");
	return std::make_unique<SceneRoot>(scene, knownTypes, knownAnimations);
}