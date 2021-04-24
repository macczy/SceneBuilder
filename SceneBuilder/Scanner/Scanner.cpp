#include <string>  
#include <algorithm>
#include "Scanner.h"
#include "../SyntaxError.h"

SceneBuilderScanner::SceneBuilderScanner(std::istream& input) : input(input), line(1), column(0) {
	singleCharTokens = { 
		{'}', Token::TokenType::CLOSING_BRACE},
		{'{', Token::TokenType::OPENING_BRACE},
		{'(', Token::TokenType::OPENING_BRACKET},
		{')', Token::TokenType::CLOSING_BRACKET},
		{':', Token::TokenType::COLON},
		{'.', Token::TokenType::DOT},
		{',', Token::TokenType::COMMA},
		{'[', Token::TokenType::OPENING_SQUARE_BRACE},
		{']', Token::TokenType::CLOSING_SQUARE_BRACE},
		{'+', Token::TokenType::PLUS},
		{'-', Token::TokenType::MINUS},
		{'/', Token::TokenType::SLASH},
		{'*', Token::TokenType::ASTERISK},
		{'=', Token::TokenType::EQUAL_SIGN},
		{'?', Token::TokenType::QUESTION_MARK}
	};
	lambdaGeneratedTokens = {
		{'|', [=](Position tokenStartPosition) {
			if (getNextChar() == '|') {
				currentToken = Token(Token::TokenType::OR, "||", tokenStartPosition);
				getNextChar();
			}
			else {
				tokenStartPosition.columnNumber++;
				std::string errorMessage = "Expected | but got '" + std::string(1, character) + "' " + tokenStartPosition.toString();
				throw SyntaxError(errorMessage);
				}}},
		{'&', [=](Position tokenStartPosition) {
			if (getNextChar() == '&') {
				currentToken = Token(Token::TokenType::AND, "&&", tokenStartPosition);
				getNextChar();
			}
			else {
				tokenStartPosition.columnNumber++;
				std::string errorMessage = "Expected & but got '" + std::string(1, character) + "' " + tokenStartPosition.toString();
				throw SyntaxError(errorMessage);
			}}},
		{'<', [=](Position tokenStartPosition) {
			if (getNextChar() == '=') {
				currentToken = Token(Token::TokenType::LESS_OR_EQUAL, "<=", tokenStartPosition);
				getNextChar();
			}
			else
				currentToken = Token(Token::TokenType::LESS_THAN, "<", tokenStartPosition);
			}},
		{'>', [=](Position tokenStartPosition) {
			if (getNextChar() == '=') {
				currentToken = Token(Token::TokenType::GREATER_OR_EQUAL, ">=", tokenStartPosition);
				getNextChar();
			}
			else
				currentToken = Token(Token::TokenType::GREATER_THAN, ">", tokenStartPosition);
			}},
		{'!', [=](Position tokenStartPosition) {
			if (getNextChar() == '=') {
				currentToken = Token(Token::TokenType::NOT_EQUAL, "!=", tokenStartPosition);
				getNextChar();
			}
			else
				currentToken = Token(Token::TokenType::UNDEFINED, "!", tokenStartPosition);
			}}
	};
	getNextChar();
	next();
}

Token SceneBuilderScanner::getToken() {
	return currentToken;
}

bool SceneBuilderScanner::isDigit(const char character) const {
	return character >= '0' && character <= '9';
}

bool SceneBuilderScanner::isCapitalLetter(const char character) const {
	return character >= 'A' && character <= 'Z';
}

bool SceneBuilderScanner::isSmallLetter(const char character)const {
	return character >= 'a' && character <= 'z';
}

bool SceneBuilderScanner::isVariableCharacter(const char character) const {
	return isSmallLetter(character) || isCapitalLetter(character) || isDigit(character) || character == '_';
}

bool SceneBuilderScanner::isHex(const char character) const {
	return character >= 'A' && character <= 'F' || isDigit(character);
}

char SceneBuilderScanner::getNextChar() {
	character = input.get();
	++column;
	if (character == '\n') {
		++line;
		column = 0;
	}
	return character;
}

inline void SceneBuilderScanner::checkIfTokenMaxLengthReached(const unsigned int limit, const Position& tokenStartPosition, const std::string& errorMessage, const size_t length) const {
	if (length > limit) {
		throw SyntaxError(errorMessage + tokenStartPosition.toString());
	}
}

bool SceneBuilderScanner::isVariableIdentifier(Position tokenStartPosition) {
	if (isSmallLetter(character)) {
		std::string tokenValue(1, character);
		while (isVariableCharacter(getNextChar())) {
			tokenValue += character;
			checkIfTokenMaxLengthReached(MAX_NAME_LENGTH, tokenStartPosition, "Variable name exceeded maximum length ", tokenValue.size());
		}
		currentToken = Token(Token::TokenType::VARIABLE_IDENTIFIER, tokenValue, tokenStartPosition);
		return true;
	}
	return false;
}

bool SceneBuilderScanner::isTypeIdentifier(Position tokenStartPosition) {
	if (isCapitalLetter(character)) {
		std::string tokenValue(1, character);
		while (isVariableCharacter(getNextChar())) {
			tokenValue += character;
			checkIfTokenMaxLengthReached(MAX_NAME_LENGTH, tokenStartPosition, "Type name exceeded maximum length ", tokenValue.size());
		}
		currentToken = Token(Token::TokenType::TYPE_IDENTIFIER, tokenValue, tokenStartPosition);
		return true;
	}
	return false;
}

bool SceneBuilderScanner::isHexConst(Position tokenStartPosition) {
	if (character == '#') {
		std::string tokenValue(1, character);
		while (isHex(getNextChar())) {
			tokenValue += character;
			checkIfTokenMaxLengthReached(MAX_HEX_VALUE_LENGTH, tokenStartPosition, "Hexadecimal value exceeded maximum length ", tokenValue.size());
		}
		if (tokenValue.size() == 1) {
			tokenStartPosition.columnNumber++;
			std::string errorMessage = "Expected hexadecimal const value, but got '" + std::string(1, character) + "' " 
				+ tokenStartPosition.toString() + getLineError(tokenStartPosition);
			throw SyntaxError(errorMessage);
		}

		currentToken = Token(Token::TokenType::HEX_CONST, tokenValue, tokenStartPosition);
		return true;
	}
	return false;
}

std::string SceneBuilderScanner::getLineError(Position position) {
	std::string buffer;
	auto positionInInput = input.tellg();
	input.seekg(position.totalPositionNumber-position.columnNumber + (position.lineNumber==1 ? 1 : 0), std::ios::beg);
	std::getline(input, buffer);
	std::replace(buffer.begin(), buffer.end(), '\t', ' ');
	buffer = "\n" + buffer + "\n" + std::string(position.columnNumber - 1, ' ') + "^";
	input.seekg(positionInInput, std::ios::beg);
	return buffer;
}

bool SceneBuilderScanner::isDecimalConst(Position tokenStartPosition) {
	if (isDigit(character)) {
		std::string tokenValue(1, character);
		if (character == 0) { 
			if (getNextChar() == '.') {
				tokenValue += character;
				while (isDigit(getNextChar())) {
					tokenValue += character;
					checkIfTokenMaxLengthReached(MAX_DECIMAL_VALUE_LENGTH, tokenStartPosition, "Decimal value exceeded maximum length ", tokenValue.size());
				}
			}
			else {
				currentToken = Token(Token::TokenType::DECIMAL_CONST, tokenValue, tokenStartPosition);
				getNextChar();
			}
		}
		else {
			while (isDigit(getNextChar())) {
				tokenValue += character;
				checkIfTokenMaxLengthReached(MAX_DECIMAL_VALUE_LENGTH, tokenStartPosition, "Decimal value exceeded maximum length ", tokenValue.size());
			}
			if (character == '.') {
				tokenValue += character;
				while (isDigit(getNextChar())) {
					tokenValue += character;
					checkIfTokenMaxLengthReached(MAX_DECIMAL_VALUE_LENGTH, tokenStartPosition, "Decimal value exceeded maximum length ", tokenValue.size());
				}
			}
			currentToken = Token(Token::TokenType::DECIMAL_CONST, tokenValue, tokenStartPosition);
			return true;
		}
	}
	return false;
}

void SceneBuilderScanner::next() {
	try {
		unsigned long tokenLine = line;
		unsigned long tokenColumn = column;
		std::streamoff tokenPosition = input.tellg();
		Position tokenStartPosition{ tokenLine, tokenColumn, tokenPosition };
		
		unsigned long currentCharacterCountNumber = 0;
		while (isspace(character)) {
			checkIfTokenMaxLengthReached(MAX_EMPTY_SPACE_LENGTH, tokenStartPosition,
				"Expected token, got " + std::to_string(currentCharacterCountNumber) + " blank characters ", ++currentCharacterCountNumber);
			getNextChar();
		}
		tokenStartPosition = Position{ line, column, input.tellg() };
		if (input.eof()) {
			currentToken = Token(Token::TokenType::END_OF_FILE, "", tokenStartPosition);
			return;
		}
		if (isVariableIdentifier(tokenStartPosition)) return;
		if (isTypeIdentifier(tokenStartPosition)) return;
		if (isDecimalConst(tokenStartPosition)) return;
		if (isHexConst(tokenStartPosition)) return;

		auto& lambda = lambdaGeneratedTokens[character];
		if(lambda != nullptr) {
			lambda(tokenStartPosition);
		} else {
			Token::TokenType type = singleCharTokens[character]; //return UNDEFINED if character not found
			currentToken = Token(type, std::string(1, character), tokenStartPosition);
			getNextChar();
		}
	}
	catch (SyntaxError&) {
		currentToken = Token(Token::TokenType::UNDEFINED, "", Position{ line, column, input.tellg() });
		throw;
	}
}