#pragma once
#include "Scanner.h"
#include "SceneRoot.h"


class Parser
{
public:
	Parser(Scanner& scanner) : scanner(scanner), root() {

	}

	virtual SceneRoot parse();

	bool tryBuildComplesObject();
	bool tryBuildAnimationDeclaration();

private:
	Scanner& scanner;
	Token currentToken;
	SceneRoot root;
};

