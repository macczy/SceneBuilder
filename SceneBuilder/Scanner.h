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
	bool isDigit(const char character) const;
	bool isCapitalLetter(const char character) const;
	bool isSmallLetter(const char character) const;
	bool isVariableCharacter(const char character) const;
	bool isHex(const char character) const;
	bool isVariableIdentifier(Token::Position tokenStartPosition);
	bool isTypeIdentifier(Token::Position tokenStartPosition);
	bool isHexConst(Token::Position tokenStartPosition);
	bool isDecimalConst(Token::Position tokenStartPosition);
	char getNextChar();
	inline void checkIfTokenMaxLengthReached(const unsigned int limit, const Token::Position& tokenStartPosition, const std::string& errorMessage, const size_t length) const;
public:
	Scanner(std::istream& input);
	virtual ~Scanner() {}
	Token virtual getToken();
	void virtual next();
	static const unsigned int MAX_NAME_LENGTH = 1024;
	static const unsigned int MAX_EMPTY_SPACE_LENGTH = 1024;
	static const unsigned int MAX_HEX_VALUE_LENGTH = 32;
	static const unsigned int MAX_DECIMAL_VALUE_LENGTH = 32;
};

