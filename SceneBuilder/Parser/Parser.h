#pragma once
#include <optional>
#include "SceneRoot.h"
#include "../objects/Color.h"
#include "../objects/Point.h"
#include "../Scanner/Scanner.h"

class Parser
{
public:
	Parser(Scanner& scanner);

	virtual std::unique_ptr<SceneRoot> parse();

	bool tryBuildComplexObject();
	bool tryBuildAnimationDeclaration();
	bool tryBuildComplexObjectDeclaration();
	bool tryBuildVariableObject();
	bool tryBuildComplexOrSimpleObject();
	bool tryBuildKnownType();
	std::optional<Color> tryBuildColor();
	std::optional<Point> tryBuildPoint();
	std::optional<std::string> getDecimalValue();


	//helper functions:
	std::string errorExpectingXgotYat(const std::string& expected, const std::string& got, const Position& pos);
private:
	Scanner& scanner;
	Token currentToken;
	std::unique_ptr<SceneRoot> root;
	std::vector<std::string> knownTypes;
	const Token& getNextToken();
	bool isKnownType(const std::string& value);
};

