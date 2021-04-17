#pragma once
#include "Scanner.h"
#include "SceneRoot.h"


class Parser
{
public:
	Parser(Scanner& scanner) : scanner(scanner), root(new SceneRoot()) {

	}

	virtual std::unique_ptr<SceneRoot> parse();

	bool tryBuildComplesObject();
	bool tryBuildAnimationDeclaration();

private:
	Scanner& scanner;
	Token currentToken;
	std::unique_ptr<SceneRoot> root;
};

