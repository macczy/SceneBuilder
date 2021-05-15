#include "Parser.h"
#include "../SyntaxError.h"
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

ComplexObjectDeclarationPtr Parser::tryBuildNewComplexObject(const Token& identifierToken) {//'Identifier =' should already be found by more general function, this only checks what's after
    if (currentToken.getType() != TokenType::OPENING_BRACE) return nullptr;
    Objects objects;
    Properties properties;
    getNextToken();
    while (auto ident = tryBuildIdentifier()) {
        if (auto property = tryBuildProperty(*ident); property) {
            properties.push_back(std::move(property));
        }
        else {
            if (currentToken.getType() != TokenType::EQUAL_SIGN) throwSyntaxError("= or :", currentToken);
            getNextToken();
            if (auto obj = tryBuildBasicObject()) {
                objects.push_back(std::make_unique<ObjectIdentifierPair>(*ident, obj));
            }
            else if (auto obj = tryBuildComplexObject()) {
                objects.push_back(std::make_unique<ObjectIdentifierPair>(*ident, obj));
            }
            else {//try build animation
                throwSyntaxError("object declaration", currentToken);
            }
        }
    }
    if (currentToken.getType() != TokenType::CLOSING_BRACE) throwSyntaxError("closing bracket or identifier", currentToken);
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

AnimationDeclarationPtr Parser::tryBuildAnimationDeclaration(const Token& identifierToken) {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) return nullptr;
    if(currentToken.getValue() != "AnimationSequence") return nullptr;
    if (getNextToken().getType() != TokenType::OPENING_BRACKET) throwSyntaxError("opening bracket", currentToken);
    std::vector<Identifier> args;
    do {
        if (getNextToken().getType() != TokenType::VARIABLE_IDENTIFIER) throwSyntaxError("identifier", currentToken);
        args.push_back(Identifier(currentToken.getPosition(), currentToken.getValue()));
    } while (getNextToken().getType() == TokenType::COMMA);
    if (currentToken.getType() != TokenType::CLOSING_BRACKET) throwSyntaxError("opening bracket", currentToken);
    if (getNextToken().getType() != TokenType::OPENING_BRACE) throwSyntaxError("opening brace", currentToken);
    getNextToken();
    Properties properties;
    std::vector<AnimationPtr> animations;
    bool build = false;
    do {
        build = false;
        if (auto animation = tryBuildAnimation(); animation) {
            animations.push_back(std::move(animation));
            build = true;
        }
        if (currentToken.getType() == TokenType::VARIABLE_IDENTIFIER) {
            Identifier ident(currentToken.getPosition(), currentToken.getValue());
            getNextToken();
            if (auto property = tryBuildProperty(ident); property) {
                properties.push_back(std::move(property));
                build = true;
            }
            else
                throwSyntaxError("property", currentToken);
        }
    }while (build);
    if (currentToken.getType() != TokenType::CLOSING_BRACE) throwSyntaxError("closing brace", currentToken);
    getNextToken();
    return std::make_unique<AnimationDeclaration>(identifierToken.getPosition(), identifierToken.getValue(), args, properties, animations);
}

AnimationPtr Parser::tryBuildWait() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) return nullptr;
    if (currentToken.getValue() != "Wait") return nullptr;
    auto startingToken = currentToken;
    if (getNextToken().getType() != TokenType::OPENING_BRACE) throwSyntaxError("opening brace", currentToken);
    getNextToken();
    Properties properties;
    while (auto ident = tryBuildIdentifier()) {
        //if (currentToken.getType() != TokenType::COLON) throwSyntaxError("colon", currentToken);
        //getNextToken();
        if (auto property = tryBuildProperty(*ident); property) {
            properties.push_back(std::move(property));
        }
    }
    if (currentToken.getType() != TokenType::CLOSING_BRACE) throwSyntaxError("closing brace", currentToken);
    getNextToken();
    return std::make_unique<Wait>(startingToken.getPosition(), properties);
}
AnimationPtr Parser::tryBuildComplexAnimation() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) return nullptr;
    if (!(currentToken.getValue() == "ParalelAnimation" || currentToken.getValue() == "AnimationSequence")) return nullptr;
    auto startingToken = currentToken;
    if (getNextToken().getType() != TokenType::OPENING_BRACE) throwSyntaxError("opening brace", currentToken);
    getNextToken();
    Properties properties;
    std::vector<AnimationPtr> animations;
    bool build = false;
    do {
        build = false;
        if (auto animation = tryBuildAnimation(); animation) {
            animations.push_back(std::move(animation));
            build = true;
        }
        if (currentToken.getType() == TokenType::VARIABLE_IDENTIFIER) {
            Identifier ident(currentToken.getPosition(), currentToken.getValue());
            getNextToken();
            if (auto property = tryBuildProperty(ident); property) {
                properties.push_back(std::move(property));
                build = true;
            }
            else
                throwSyntaxError("property", currentToken);
        }
    } while (build);
    if (currentToken.getType() != TokenType::CLOSING_BRACE) throwSyntaxError("closing brace", currentToken);
    getNextToken();
    if(startingToken.getValue() == "ParalelAnimation")
        return std::make_unique<ParalelAnimation>(startingToken.getPosition(), properties, animations);
    return std::make_unique<AnimationSequence>(startingToken.getPosition(), properties, animations);
}
AnimationPtr Parser::tryBuildConditionalAnimation() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) return nullptr;
    if (currentToken.getValue() != "ConditionalAnimation") return nullptr;
    auto startingToken = currentToken;
    if (getNextToken().getType() != TokenType::OPENING_BRACE) throwSyntaxError("opening brace", currentToken);
    getNextToken();
    Properties properties;
    LogicalSubExpressionPtr log;
    std::vector<AnimationPtr> animations;    
    bool build = false;
    do {
        build = false;
        if (auto animation = tryBuildAnimation(); animation) {
            animations.push_back(std::move(animation));
            build = true;
        }
        if (currentToken.getType() == TokenType::VARIABLE_IDENTIFIER) {
            build = true;
            Identifier ident(currentToken.getPosition(), currentToken.getValue());
            getNextToken();
            if (ident.getValue() == "condition") {
                if (log) throw SyntaxError("Redefinition of condition property " + 
                    currentToken.getPosition().toString() + "\n" + scanner.getLineError(currentToken.getPosition()));
                if (currentToken.getType() != TokenType::COLON) throwSyntaxError(":", currentToken);
                getNextToken();
                if (auto inBrackets = tryBuildLogicalExpressionInBrackets(); inBrackets) {
                    if (auto subExpr1 = tryBuildLogicalExpression(inBrackets); subExpr1)
                        log = std::make_unique<LogicalSubExpression>(std::move(subExpr1));
                    else
                        log = std::move(inBrackets);
                }
                else {
                    auto exp = tryBuildValue();
                    if (!exp) throwSyntaxError("logical expression", currentToken);
                    if (auto comparison = tryBuildComparison(*exp); comparison) {
                        auto subExpr = std::make_unique<LogicalSubExpression>(std::move(comparison));
                        if (auto subExpr1 = tryBuildLogicalExpression(subExpr); subExpr1)
                            log = std::make_unique<LogicalSubExpression>(std::move(subExpr1));
                        else
                            log = std::move(subExpr);
                    }
                    else {
                        throwSyntaxError("logical expression", currentToken);
                    }
                }
            } else if (auto property = tryBuildProperty(ident); property) {
                properties.push_back(std::move(property));
            }
            else
                throwSyntaxError("property", currentToken);
        }
    } while (build);
    if (currentToken.getType() != TokenType::CLOSING_BRACE) throwSyntaxError("closing brace", currentToken);
    getNextToken();
    return std::make_unique<ConditionalAnimation>(startingToken.getPosition(), properties, animations, log);
}

AnimationPtr Parser::tryBuildBasicAnimation() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) return nullptr;
    if (currentToken.getValue() != "Animation") return nullptr;
    auto startingToken = currentToken;
    if (getNextToken().getType() != TokenType::OPENING_BRACE) throwSyntaxError("opening brace", currentToken);
    getNextToken();
    Properties properties;
    while (auto ident = tryBuildIdentifier()) {
        if (auto property = tryBuildProperty(*ident); property) {
            properties.push_back(std::move(property));
        }
    }
    if (currentToken.getType() != TokenType::CLOSING_BRACE) throwSyntaxError("closing brace", currentToken);
    getNextToken();
    return std::make_unique<Animation>(startingToken.getPosition(), properties);
}

AnimationPtr Parser::tryBuildAnimation() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) return nullptr;
    if(!AnimationFactory::isAnimationType(currentToken.getValue())) return nullptr;
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
        throwSyntaxError("'('", currentToken);
    if (getNextToken().getType() != TokenType::HEX_CONST) 
        throwSyntaxError("hexadecimal value", currentToken);
    auto firstHexValue = currentToken.getValue();
    if (getNextToken().getType() != TokenType::COMMA) 
        throwSyntaxError("','", currentToken);
    if (getNextToken().getType() != TokenType::HEX_CONST) 
        throwSyntaxError("hexadecimal value", currentToken);
    auto secondHexValue = currentToken.getValue();
    if (getNextToken().getType() != TokenType::COMMA) 
        throwSyntaxError("','", currentToken);
    if (getNextToken().getType() != TokenType::HEX_CONST) 
        throwSyntaxError("hexadecimal value", currentToken);
    auto thirdHexValue = currentToken.getValue();
    if (getNextToken().getType() != TokenType::CLOSING_BRACKET) 
        throwSyntaxError("')'", currentToken);
    getNextToken();
    return Color(startingColorPosition, firstHexValue, secondHexValue, thirdHexValue);
}

std::optional<DecimalValue> Parser::tryBuildDecimalValue() {
    Position pos = currentToken.getPosition();
    if (currentToken.getType() == TokenType::MINUS) {
        if (getNextToken().getType() != TokenType::DECIMAL_CONST) 
            throwSyntaxError("decimal value", currentToken);
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

void Parser::throwSyntaxError(const std::string& expected, const std::string& got, Token& token) {
    throw SyntaxError(expected, got, token.getPosition(), [=](const Position& pos) {
        return scanner.getLineError(pos);
        });
}

void Parser::throwSyntaxError(const std::string& expected, Token& token) {
    throw SyntaxError(expected, token.getValue(), token.getPosition(), [=](const Position& pos) {
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
            throwSyntaxError("variable identifier after '.'", currentToken);
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
        throwSyntaxError("'('", currentToken);
    if (getNextToken().getType() == TokenType::CLOSING_BRACKET) {
        getNextToken();
        return Point(startingPointPosition);
    }

    auto firstDecimalValue = tryBuildDecimalValue();
    if (!firstDecimalValue) 
        throwSyntaxError("')' or decimal value", currentToken);

    if (currentToken.getType() != TokenType::COMMA) {
        if (currentToken.getType() == TokenType::CLOSING_BRACKET) {
            getNextToken();
            return Point(startingPointPosition, *firstDecimalValue);
        }
        throwSyntaxError("',' or ')'", currentToken);
    }
    getNextToken();
    auto secondDecimalValue = tryBuildDecimalValue();
    if (!secondDecimalValue)
        throwSyntaxError("decimal value", currentToken);

    if (currentToken.getType() != TokenType::COMMA) {
        if (currentToken.getType() == TokenType::CLOSING_BRACKET) {
            getNextToken();
            return Point(startingPointPosition, *firstDecimalValue);
        }
        throwSyntaxError("',' or ')'", currentToken);
    }
    getNextToken();
    auto thirdDecimalValue = tryBuildDecimalValue();
    if (!thirdDecimalValue) throwSyntaxError("decimal value", currentToken);
        
    if (currentToken.getType() != TokenType::CLOSING_BRACKET) throwSyntaxError("')'", currentToken);

    getNextToken();
    return Point(startingPointPosition, *firstDecimalValue, *secondDecimalValue, *thirdDecimalValue);
}

std::optional<TimeDeclaration> Parser::tryBuildTimeDeclaration() {
    if (currentToken.getType() != TokenType::DOUBLE_QUOTE_CHARACTER) return std::nullopt;
    auto pos = currentToken.getPosition();
    if (getNextToken().getType() != TokenType::DECIMAL_CONST) throwSyntaxError("time declaration", currentToken);
    auto decValue = currentToken.getValue();    
    auto timeSpecifier = getNextToken().getValue();
    if (currentToken.getType() != TokenType::VARIABLE_IDENTIFIER || !TimeDeclaration::isCorrectTimeSpecifier(timeSpecifier)) throwSyntaxError("time specifier", currentToken);
    if (getNextToken().getType() != TokenType::DOUBLE_QUOTE_CHARACTER) throwSyntaxError("time declaration closing quote", currentToken);
    getNextToken();
    return TimeDeclaration(pos, decValue, timeSpecifier);
}

std::optional<ConstantIdentifier> Parser::tryBuildConstantIdentifier() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER)
        return std::nullopt;

    std::vector<std::pair<Position, std::string>> identifiers;
    identifiers.push_back(std::make_pair(currentToken.getPosition(), currentToken.getValue()));
    while (getNextToken().getType() == TokenType::DOT) {
        if (getNextToken().getType() != TokenType::TYPE_IDENTIFIER)
            throwSyntaxError("type identifier after '.'", currentToken);
        identifiers.push_back(std::make_pair(currentToken.getPosition(), currentToken.getValue()));
    }
    if (identifiers.size() == 1)
        return ConstantIdentifier(identifiers[0].first, identifiers[0].second);

    auto identifier_ptr = std::make_unique<ConstantIdentifier>(identifiers.back().first, identifiers.back().second);
    identifiers.pop_back();
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
    do {
        if(!points.empty()) getNextToken();
        if (auto point = tryBuildPoint(); point)
            points.push_back(*point);
        else {
            if (points.empty()) return std::nullopt;
            throwSyntaxError("vertex declaration", currentToken);
        }
    } while (currentToken.getType() == TokenType::COMMA);
    if (currentToken.getType() != TokenType::CLOSING_SQUARE_BRACE) throwSyntaxError("closing square brace", currentToken);
    getNextToken();
    return PointArray(pos, points);
}


Argument Parser::tryBuildArgument() {
    Argument arg;
    if (currentToken.getType() != TokenType::OPENING_SQUARE_BRACE) return arg;
    do {
        getNextToken();
        if (auto expr = tryBuildExpression(); expr) {
            arg.push_back(std::move(*expr));
        }
        else if (!arg.empty()) throwSyntaxError("expression", currentToken);
    } while (currentToken.getType() == TokenType::COMMA);
    if (currentToken.getType() != TokenType::CLOSING_SQUARE_BRACE) throwSyntaxError("closing square bracket", currentToken);
    getNextToken();
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
        else if (!args.empty()) throwSyntaxError("expression", currentToken);
    } while (currentToken.getType() == TokenType::COMMA);
    if (currentToken.getType() != TokenType::CLOSING_BRACKET) throwSyntaxError("closing bracket", currentToken);
    getNextToken();
    return std::move(args);
}

AnimationCallPtr Parser::tryBuildAnimationCall() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) return nullptr;
    auto startToken = currentToken;
    if (getNextToken().getType() != TokenType::OPENING_BRACKET) throwSyntaxError("opening bracket", currentToken);
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
            throwSyntaxError("animation call declaration", currentToken);
        }
    } while (currentToken.getType() == TokenType::COMMA);

    if (currentToken.getType() != TokenType::CLOSING_SQUARE_BRACE) throwSyntaxError("closing square brace", currentToken);
    getNextToken();
    return AnimationProperty(pos, animationCalls);
}

std::optional<Expression> Parser::tryBuildArrayType() {
    if (currentToken.getType() != TokenType::OPENING_SQUARE_BRACE) return std::nullopt;
    const Position& pos = currentToken.getPosition();
    getNextToken();
    if (currentToken.getType() == TokenType::CLOSING_SQUARE_BRACE) throw SyntaxError("array cannot be empty " + currentToken.getPosition().toString() + scanner.getLineError(currentToken.getPosition()));
    if (auto points = tryBuildPointArray(pos); points) {
        return points;
    }
    if (auto anim = tryBuildAnimationProperty(pos); anim) {
        return anim;
    }
    throwSyntaxError("array", currentToken);
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
            throwSyntaxError("expression", "empty brackets", openingToken);
        }
        throwSyntaxError("closing bracket", currentToken.getValue(), openingToken);
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
                throwSyntaxError("question mark", currentToken);
            }
            if (auto ternary = tryBuildTernaryExpression(subExpr); ternary)
                return ternary;
            throwSyntaxError("question mark", currentToken);
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
            throwSyntaxError("colon", currentToken);
        getNextToken();
        if (auto ifFalse = tryBuildExpression(); ifFalse) {
            return std::make_unique<TernaryExpression>(getLogicalSubExpressionPosition(*condition), condition, *ifTrue, *ifFalse);
        }
        throwSyntaxError("expression", currentToken);
    }
    throwSyntaxError("expression", currentToken);
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
                    throwSyntaxError("closing bracket", currentToken);
                }
                if (currentToken.getType() == TokenType::CLOSING_BRACKET) {
                    getNextToken();
                    return subExpr;
                }
                throwSyntaxError("closing bracket", currentToken);
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
    if(currentToken.getType() != TokenType::COLON) return nullptr;
    getNextToken();
    if (auto expr = tryBuildExpression(); expr) {
        return std::make_unique<Property>(ident.getPosition(), ident, *expr);
    }
    throwSyntaxError("expression", currentToken);
}


BasicObjectPtr Parser::tryBuildBasicObject() {
    if (!BasicObjectFactory::isBasicObjectNameToken(currentToken)) return nullptr;
    auto startingToken = currentToken;
    Properties properties;
    getNextToken();
    if(currentToken.getType() != TokenType::OPENING_BRACE) throwSyntaxError("{", currentToken);
    getNextToken(); 
    while(auto ident = tryBuildIdentifier()) {
        if (auto property = tryBuildProperty(*ident); property) {
            properties.push_back(std::move(property));
        }
        else throwSyntaxError("expression", currentToken);
    }
    if (currentToken.getType() != TokenType::CLOSING_BRACE) throwSyntaxError("}", currentToken);
    getNextToken();
    return BasicObjectFactory::getBasicObject(startingToken.getPosition(), properties, startingToken);
}

ComplexObjectPtr Parser::tryBuildComplexObject()
{
    if (BasicObjectFactory::isBasicObjectNameToken(currentToken)) return nullptr;
    if (!isKnownType(currentToken.getValue())) return nullptr;
    auto startingToken = currentToken;
    Properties properties;
    getNextToken();
    if (currentToken.getType() != TokenType::OPENING_BRACE) throwSyntaxError("{", currentToken);
    getNextToken();
    while (auto ident = tryBuildIdentifier()) {
        if (auto property = tryBuildProperty(*ident); property) {
            properties.push_back(std::move(property));
        }
        else
            throwSyntaxError("property declaration", currentToken);
    }
    if (currentToken.getType() != TokenType::CLOSING_BRACE) throwSyntaxError("}", currentToken);
    getNextToken();
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
    throwSyntaxError("value", currentToken.getValue(), operatorToken);
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
    throwSyntaxError("value after " + comparisonToken.getValue(), nextToken.getValue(), nextToken);
}


ScenePtr Parser::tryBuildScene() {
    if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) return nullptr;
    if (currentToken.getValue() != "Scene") return nullptr;
    auto pos = currentToken.getPosition();
    if (getNextToken().getType() != TokenType::OPENING_BRACE) throwSyntaxError("opening brace", currentToken);
    getNextToken();
    Objects objects;
    Properties properties;
    while (auto ident = tryBuildIdentifier()) {
        if (auto property = tryBuildProperty(*ident); property) {
            properties.push_back(std::move(property));
        }
        else {
            if (currentToken.getType() != TokenType::EQUAL_SIGN) throwSyntaxError("= or :", currentToken);
            getNextToken();
            if (auto obj = tryBuildBasicObject(); obj) {
                objects.push_back(std::make_unique<ObjectIdentifierPair>(*ident, obj));
            }
            else if (auto obj = tryBuildComplexObject(); obj) {
                objects.push_back(std::make_unique<ObjectIdentifierPair>(*ident, obj));
            } else throwSyntaxError("object declaration", currentToken);
        }
    }
    if (currentToken.getType() != TokenType::CLOSING_BRACE) throwSyntaxError("closing brace, property or sub object declaration", currentToken);
    getNextToken();
    return std::make_unique<Scene>(pos, objects, properties);
}

std::unique_ptr<SceneRoot> Parser::parse() {
    try {
        while (currentToken.getType() != TokenType::END_OF_FILE && !scene) {
            if (currentToken.getType() != TokenType::TYPE_IDENTIFIER) throwSyntaxError("expected animation, complex object, or scene declaration", currentToken);
            auto ident = currentToken;
            if (auto scene = tryBuildScene(); scene) {
                this->scene = std::move(scene);
            }
            else {
                if (getNextToken().getType() != TokenType::EQUAL_SIGN)
                    throwSyntaxError("expected animation, complex object, or scene declaration", currentToken);
                getNextToken();
                if (auto anim = tryBuildAnimationDeclaration(ident); anim) {
                    if (isKnownAnimation(ident.getValue())) throw SyntaxError("Redefinition of " + ident.getValue() +
                        std::string(" ", 1) + ident.getPosition().toString() + scanner.getLineError(ident.getPosition()));
                    knownAnimations.push_back(std::move(anim));
                }
                else if (auto obj = tryBuildNewComplexObject(ident); obj) {
                    if (isKnownType(ident.getValue())) throw SyntaxError("Redefinition of " + ident.getValue() +
                        std::string(" ", 1) + ident.getPosition().toString() + scanner.getLineError(ident.getPosition()));
                    knownTypes.push_back(std::move(obj));
                }
                else throwSyntaxError("expected animation, complex object, or scene declaration", currentToken);
            }
        }
    }
    catch (SyntaxError er) {
        std::cout << er.what();
        std::cout << std::endl;
    }
    catch (...) {
        std::cout << "Unknown exception" << std::endl;
        throw;
    }
	return std::make_unique<SceneRoot>(scene, knownTypes, knownAnimations);
}