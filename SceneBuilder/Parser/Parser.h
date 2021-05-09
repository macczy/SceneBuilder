#pragma once
#include <optional>
#include <variant>
#include "SceneRoot.h"
#include "../Objects/Expression.h"
#include "../Scanner/Scanner.h"
#include "../Scanner/Token.h"


class Parser
{
public:
	Parser(Scanner& scanner);

	virtual std::unique_ptr<SceneRoot> parse();

	//these will be replaced later
	bool tryBuildComplexObject();
	bool tryBuildAnimationDeclaration();
	bool tryBuildComplexObjectDeclaration();
	bool tryBuildVariableObject();
	bool tryBuildComplexOrSimpleObject();
	bool tryBuildKnownType();
	//each tryBuild function assumes that current token should be chacked, and leaves 1 token that it didn't include
	//testing now

	std::optional<Expression> tryBuildValue();
	std::optional<Expression> tryBuildExpression();
	std::optional<Expression> tryBuildBracedExpression();
	//tested
	std::optional<Color> tryBuildColor();
	std::optional<Point> tryBuildPoint();
	std::optional<DecimalValue> tryBuildDecimalValue();
	std::optional<Identifier> tryBuildIdentifier();
	std::unique_ptr<Comparison> tryBuildComparison(Expression& firstValue);
	std::unique_ptr<LogicalExpression> tryBuildLogicalExpression(std::unique_ptr<LogicalSubExpression>& firstValue);
	std::unique_ptr<Addition> tryBuildAddition(Expression& firstValue);
	std::unique_ptr<Multiplication> tryBuildMultiplication(Expression& firstValue);

private:
	Scanner& scanner;
	Token currentToken;
	std::unique_ptr<SceneRoot> root;
	std::vector<std::string> knownTypes;
	const Token& getNextToken();
	bool isKnownType(const std::string& value);

	//helper functions:
	inline void throwSyntaxError(const std::string& got, const std::string& expected, Token& token );
};

