#pragma once

#include <iostream>
#include <vector>
#include "Token.h"


class Scanner
{
private:
	std::istream& input;
	Token currentToken;
	unsigned long line;
	unsigned long position;
	bool isNumber(char character);
	bool isCapitalLetter(char character);
	bool isSmallLetter(char character);
	bool isVariableCharacter(char character);
	bool isHex(char character);
	char getNextChar();
public:
	Scanner(std::istream& input);
	Token getToken();
	void next();
};

