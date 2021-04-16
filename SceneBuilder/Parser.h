#pragma once
#include "Scanner.h"


class Parser
{
	Parser(Scanner& scanner) : scanner(scanner) {

	}

private:
	Scanner& scanner;
};

