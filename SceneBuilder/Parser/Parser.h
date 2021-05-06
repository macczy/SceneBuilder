#pragma once
#include <optional>
#include <variant>
#include "SceneRoot.h"
#include "../Objects/Value.h"
#include "../Objects/Addition.h"
#include "../Objects/Multiplication.h"
#include "../Scanner/Scanner.h"
#include "../Scanner/Token.h"
#include "../Objects/LogicalExpression.h"


class Parser
{
public:
	Parser(Scanner& scanner);

	virtual std::unique_ptr<SceneRoot> parse();

	//there will be replaced later
	bool tryBuildComplexObject();
	bool tryBuildAnimationDeclaration();
	bool tryBuildComplexObjectDeclaration();
	bool tryBuildVariableObject();
	bool tryBuildComplexOrSimpleObject();
	bool tryBuildKnownType();
	//each tryBuild function assumes that current token should be chacked, and leaves 1 token that it didn't include
	//testing now

	//this
	std::optional<Value> tryBuildValue();
	//tested
	std::optional<Color> tryBuildColor();
	std::optional<Point> tryBuildPoint();
	std::optional<DecimalValue> tryBuildDecimalValue();
	std::optional<Identifier> tryBuildIdentifier();

	std::unique_ptr<Comparison> tryBuildComparison(Value& firstValue);

	std::unique_ptr<Addition> tryBuildAddition(Value& firstValue);
	std::unique_ptr<Multiplication> tryBuildMultiplication(Value& firstValue);
	std::unique_ptr<LogicalExpression> tryBuildLogicalExpression(std::unique_ptr<LogicalSubExpression>& firstValue);


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

