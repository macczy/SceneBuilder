#pragma once

#include <iostream>
#include <vector>
#include "Token.h"

class Scanner
{
private:
	char character;
	std::istream& input;
	Token currentToken;
	unsigned long line;
	unsigned long column;
	unsigned long position;
	bool isNumber(char character);
	bool isCapitalLetter(char character);
	bool isSmallLetter(char character);
	bool isVariableCharacter(char character);
	bool isHex(char character);
	bool tryBuildValueIdentifier();
	bool tryBuildTypeIdentifier();
	bool tryBuildHexConst();
	bool tryBuildDecimalConst();
	char getNextChar();
public:
	Scanner(std::istream& input);
	Token getToken();
	void next();
	static const unsigned int MAX_NAME_LENGTH = 1024;
	static const unsigned int MAX_EMPTY_SPACE_LENGTH = 1024;
	static const unsigned int MAX_HEX_VALUE_LENGTH = 32;
	static const unsigned int MAX_DECIMAL_VALUE_LENGTH = 32;
};

