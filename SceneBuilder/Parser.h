#pragma once
#include "Scanner.h"
#include "SceneRoot.h"


class Parser
{
public:
	Parser(Scanner& scanner) : scanner(scanner), root() {

	}

	virtual SceneRoot parse();

private:
	Scanner& scanner;
	SceneRoot root;
};

