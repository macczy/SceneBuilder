#include <string>  
#include "Scanner.h"
#include "SyntaxError.h"

Scanner::Scanner(std::istream& input) : input(input), line(0), column(0), position(0){
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
		{'|', [=](std::string& tokenValue, Token::Position& tokenStartPosition) {
			if (getNextChar() == '|') {
				tokenValue += character;
				currentToken = Token(Token::TokenType::OR, tokenValue, tokenStartPosition);
				getNextChar();
			}
			else {
				tokenStartPosition.columnNumber++;
				std::string errorMessage = "Expected | but got '" + std::string(1, character) + "' " + tokenStartPosition.toString();
				throw SyntaxError(errorMessage);
				}}},
		{'&', [=](std::string& tokenValue, Token::Position& tokenStartPosition) {
			if (getNextChar() == '&') {
				tokenValue += character;
				currentToken = Token(Token::TokenType::AND, tokenValue, tokenStartPosition);
				getNextChar();
			}
			else {
				tokenStartPosition.columnNumber++;
				std::string errorMessage = "Expected & but got '" + std::string(1, character) + "' " + tokenStartPosition.toString();
				throw SyntaxError(errorMessage);
			}}},
		{'<', [=](std::string& tokenValue, Token::Position& tokenStartPosition) {
			if (getNextChar() == '=') {
				tokenValue += character;
				currentToken = Token(Token::TokenType::LESS_OR_EQUAL, tokenValue, tokenStartPosition);
				getNextChar();
			}
			else
				currentToken = Token(Token::TokenType::LESS_THAN, tokenValue, tokenStartPosition);
			}},
		{'>', [=](std::string& tokenValue, Token::Position& tokenStartPosition) {
			if (getNextChar() == '=') {
				tokenValue += character;
				currentToken = Token(Token::TokenType::GREATER_OR_EQUAL, tokenValue, tokenStartPosition);
				getNextChar();
			}
			else
				currentToken = Token(Token::TokenType::GREATER_THAN, tokenValue, tokenStartPosition);
			}},
		{'!', [=](std::string& tokenValue, Token::Position& tokenStartPosition) {
			if (getNextChar() == '=') {
				tokenValue += character;
				currentToken = Token(Token::TokenType::NOT_EQUAL, tokenValue, tokenStartPosition);
				getNextChar();
			}
			else
				currentToken = Token(Token::TokenType::UNDEFINED, tokenValue, tokenStartPosition);
			}}
	};



	character = getNextChar();
	next();
}

Token Scanner::getToken() {
	return currentToken;
}

bool Scanner::isDigit(const char character) const {
	return character >= '0' && character <= '9';
}

bool Scanner::isCapitalLetter(const char character) const {
	return character >= 'A' && character <= 'Z';
}

bool Scanner::isSmallLetter(const char character)const {
	return character >= 'a' && character <= 'z';
}

bool Scanner::isVariableCharacter(const char character) const {
	return isSmallLetter(character) || isCapitalLetter(character) || isDigit(character) || character == '_';
}

bool Scanner::isHex(const char character) const {
	return character >= 'A' && character <= 'F' || isDigit(character);
}

char Scanner::getNextChar() {
	character = input.get();
	++column;
	if (character == '\n') {
		++line;
		column = 1;
	}
	++position;
	return character;
}


inline void Scanner::checkIfTokenMaxLengthReached(const unsigned int limit, const Token::Position& tokenStartPosition, const std::string& errorMessage, const size_t length) const {
	if (length > limit) {
		throw SyntaxError(errorMessage + tokenStartPosition.toString());
	}
}


bool Scanner::isVariableIdentifier(Token::Position tokenStartPosition) {
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

bool Scanner::isTypeIdentifier(Token::Position tokenStartPosition) {
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
bool Scanner::isHexConst(Token::Position tokenStartPosition) {
	if (character == '#') {
		std::string tokenValue(1, character);
		while (isHex(getNextChar())) {
			tokenValue += character;
			checkIfTokenMaxLengthReached(MAX_HEX_VALUE_LENGTH, tokenStartPosition, "Hexadecimal value exceeded maximum length ", tokenValue.size());
		}
		if (tokenValue.size() == 1) {
			std::string errorMessage = "Expected hexadecimal const value, but got '" + std::string(1, character) + "' " 
				+ tokenStartPosition.toString() + getLineError(tokenStartPosition);
			throw SyntaxError(errorMessage);
		}

		currentToken = Token(Token::TokenType::HEX_CONST, tokenValue, tokenStartPosition);
		return true;
	}
	return false;
}

std::string Scanner::getLineError(Token::Position position) {
	std::string buffer;
	auto positionInInput = input.tellg();
	input.seekg(position.totalPositionNumber-position.columnNumber);
	std::getline(input, buffer);
	std::replace(buffer.begin(), buffer.end(), '\t', ' ');
	buffer = "\n" + buffer + "\n" + std::string(position.columnNumber - 1, ' ') + "^";
	input.seekg(positionInInput);
	return buffer;
}

bool Scanner::isDecimalConst(Token::Position tokenStartPosition) {
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

void Scanner::next() {
	try {
		unsigned long tokenLine = line;
		unsigned long tokenColumn = column;
		unsigned long tokenPosition = position;
		Token::Position tokenStartPosition{ line, column, position };
		
		unsigned long currentCharacterCountNumber = 0;
		while (isspace(character)) {
			checkIfTokenMaxLengthReached(MAX_EMPTY_SPACE_LENGTH, tokenStartPosition,
				"Expected token, got " + std::to_string(currentCharacterCountNumber) + " blank characters ", ++currentCharacterCountNumber);
			getNextChar();
		}
		tokenStartPosition = Token::Position{ line, column, position };
		if (input.eof()) {
			currentToken = Token(Token::TokenType::END_OF_FILE, "", tokenStartPosition);
			return;
		}
		std::string tokenValue(1, character);
		if (isVariableIdentifier(tokenStartPosition)) return;
		if (isTypeIdentifier(tokenStartPosition)) return;
		if (isDecimalConst(tokenStartPosition)) return;
		if (isHexConst(tokenStartPosition)) return;

		auto& lambda = lambdaGeneratedTokens[character];
		if(lambda != nullptr) {
			lambda(tokenValue, tokenStartPosition);
		} else {
			Token::TokenType type = singleCharTokens[character];
			currentToken = Token(type, tokenValue, tokenStartPosition);
			getNextChar();
		}
	}
	catch (SyntaxError&) {
		currentToken = Token(Token::TokenType::UNDEFINED, "", Token::Position{ line, column, position });
		throw;
	}
}