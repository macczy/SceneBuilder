#pragma once
#include <optional>
#include <variant>
#include <vector>
#include "SceneRoot.h"
#include "../Objects/Expression.h"
#include "../Objects/ComplexObject.h"
#include "../Objects/AnimationDeclaration.h"
#include "../Scanner/Scanner.h"
#include "../Scanner/Token.h"

class Parser {
public:
	bool tryBuildMainObject();
	Parser(Scanner& scanner);
	[[nodiscard]] Arguments tryBuildArguments();
	[[nodiscard]] Argument tryBuildArgument();
	[[nodiscard]] std::unique_ptr<SceneRoot> parse();
	[[nodiscard]] AnimationPtr tryBuildAnimation();
	[[nodiscard]] AnimationPtr tryBuildWait();
	[[nodiscard]] AnimationPtr tryBuildComplexAnimation();
	[[nodiscard]] AnimationPtr tryBuildConditionalAnimation();
	[[nodiscard]] AnimationPtr tryBuildBasicAnimation();
	[[nodiscard]] AnimationDeclarationPtr tryBuildAnimationDeclaration(const Token& identifierToken);
	[[nodiscard]] ScenePtr tryBuildScene();
	[[nodiscard]] std::optional<TimeDeclaration> tryBuildTimeDeclaration();
	[[nodiscard]] ComplexObjectDeclarationPtr tryBuildNewComplexObject(const Token& identifierToken);
	[[nodiscard]] std::optional<Expression> tryBuildArrayType();
	[[nodiscard]] AnimationCallPtr tryBuildAnimationCall();
	[[nodiscard]] PropertyPtr tryBuildProperty(Identifier& ident);
	[[nodiscard]] BasicObjectPtr tryBuildBasicObject();
	[[nodiscard]] ComplexObjectPtr tryBuildComplexObject();
	[[nodiscard]] std::optional<Color> tryBuildColor();
	[[nodiscard]] std::optional<Point> tryBuildPoint();
	[[nodiscard]] std::optional<DecimalValue> tryBuildDecimalValue();
	[[nodiscard]] std::optional<Identifier> tryBuildIdentifier();
	[[nodiscard]] std::optional<ConstantIdentifier> tryBuildConstantIdentifier();
	[[nodiscard]] std::optional<PointArray> tryBuildPointArray(const Position& pos);
	[[nodiscard]] std::optional<AnimationProperty> tryBuildAnimationProperty(const Position& pos);
	[[nodiscard]] std::optional<Expression> tryBuildValue();
	[[nodiscard]] std::optional<Expression> tryBuildExpression();
	[[nodiscard]] std::optional<Expression> tryBuildBrackets();
	//builds all Expressions except Comparison, LogicalExpression and TernaryExpression - unless it's in brackets
	[[nodiscard]] std::optional<Expression> tryBuildSimpleExpression();
	[[nodiscard]] ComparisonPtr tryBuildComparison(Expression& firstValue);
	[[nodiscard]] AdditionPtr tryBuildAddition(Expression& firstValue);
	[[nodiscard]] MultiplicationPtr tryBuildMultiplication(Expression& firstValue);
	[[nodiscard]] std::optional<Expression> tryBuildMultiplicationOrAddition(Expression& expr);
	[[nodiscard]] LogicalSubExpressionPtr tryBuildLogicalExpressionInBrackets();
	[[nodiscard]] LogicalExpressionPtr tryBuildLogicalExpression(LogicalSubExpressionPtr& firstValue);
    [[nodiscard]] TernaryExpressionPtr tryBuildTernaryExpression(LogicalSubExpressionPtr& condition);
	[[nodiscard]] LogicalExpressionPtr getLogicalSubExpression(const Token& operatorToken,
		LogicalSubExpressionPtr& comparison, LogicalSubExpressionPtr& firstValue);
	[[nodiscard]] LogicalSubExpressionPtr tryBuildCondition();
	[[nodiscard]] bool tryBuildAnimationPropertyOrCondition(Animations& animations, Properties& properties, LogicalSubExpressionPtr& condition);
private:
	Scanner& scanner;
	Token currentToken;
	std::unique_ptr<Scene> scene;
	std::vector<ComplexObjectDeclarationPtr> knownTypes;
	std::vector<AnimationDeclarationPtr> knownAnimations;
	//check current token type, consume if matches, otherwise throwSyntaxError
	void consumeToken(TokenType expected);
	[[nodiscard]] bool isNotCurrentToken(TokenType expected) noexcept;
	[[nodiscard]] bool tryBuildProperty(Properties& properties);
	[[nodiscard]] bool tryBuildObjectOrProperty(Properties& properties, Objects& objects);
	[[nodiscard]] bool tryBuildAnimationOrProperty(Properties& properties, Animations& animations);
	const Token& getNextToken();
	bool isKnownType(const std::string& value);
	bool isKnownAnimation(const std::string& value);

	[[noreturn]] inline void throwSyntaxError(const std::string& got, const std::string& expected, Token& token);
	[[noreturn]] inline void throwSyntaxError(const std::string& got, Token& token);
};

