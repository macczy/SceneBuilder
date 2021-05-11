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

	//tested
	std::optional<Color> tryBuildColor();
	std::optional<Point> tryBuildPoint();
	std::optional<DecimalValue> tryBuildDecimalValue();
	std::optional<Identifier> tryBuildIdentifier();
	std::optional<Expression> tryBuildValue();
	std::optional<Expression> tryBuildExpression();
	std::optional<Expression> tryBuildBrackets();
	//builds all exceptions except Comparison, LogicalExpression and TernaryExpression - unless it's in brackets
	std::optional<Expression> tryBuildSimpleExpression();
    ComparisonPtr tryBuildComparison(Expression& firstValue);
    AdditionPtr tryBuildAddition(Expression& firstValue);
    MultiplicationPtr tryBuildMultiplication(Expression& firstValue);
	std::optional<Expression> tryBuildMultiplicationOrAddition(Expression& expr);
	LogicalSubExpressionPtr tryBuildLogicalExpressionInBrackets();
    LogicalExpressionPtr tryBuildLogicalExpression(LogicalSubExpressionPtr& firstValue);
    TernaryExpressionPtr tryBuildTernaryExpression(LogicalSubExpressionPtr& condition);

	//helper functions not consuming tokens
	LogicalExpressionPtr getLogicalSubExpression(const Token& operatorToken, 
		LogicalSubExpressionPtr& comparison, LogicalSubExpressionPtr& firstValue);
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

