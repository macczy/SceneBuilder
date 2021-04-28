#pragma once

#include <iostream>
#include <map>
#include <functional>
#include "Token.h"

class Scanner {
public:
	Scanner() {}
	virtual ~Scanner() {}
	virtual Token getToken() { return Token(); }
	virtual void next() {}
	virtual std::string getLineError(Position position) { return ""; }
};

class SceneBuilderScanner : public Scanner
{
private:
	char character;
	std::istream& input;
	Token currentToken;

	std::map<char, Token::TokenType> singleCharTokens;
	std::map<char, std::function<void(Position&)>> lambdaGeneratedTokens;

	int line;
	int column;

	bool isDigit(const char character) const;
	bool isCapitalLetter(const char character) const;
	bool isSmallLetter(const char character) const;
	bool isVariableCharacter(const char character) const;
	bool isHex(const char character) const;
	bool isVariableIdentifier(Position tokenStartPosition);
	bool isTypeIdentifier(Position tokenStartPosition);
	bool isHexConst(Position tokenStartPosition);
	bool isDecimalConst(Position tokenStartPosition);
	char getNextChar();
	inline void checkIfTokenMaxLengthReached(const unsigned int limit, const Position& tokenStartPosition, const std::string& errorMessage, const size_t length) const;
public:
	SceneBuilderScanner(std::istream& input);
	virtual ~SceneBuilderScanner() {}
	virtual Token getToken();
	virtual void next();
	virtual std::string getLineError(Position position);
	static const inline unsigned int MAX_NAME_LENGTH = 1024;
	static const inline unsigned int MAX_EMPTY_SPACE_LENGTH = 1024;
	static const inline unsigned int MAX_HEX_VALUE_LENGTH = 32;
	static const inline unsigned int MAX_DECIMAL_VALUE_LENGTH = 32;
};

