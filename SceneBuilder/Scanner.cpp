#include <string>  
#include "Scanner.h"
#include "SyntaxError.h"

Scanner::Scanner(std::istream& input) : input(input), line(0), column(0), position(0){
	character = getNextChar();
	next();
}

Token Scanner::getToken() {
	return currentToken;
}

bool Scanner::isNumber(char character) {
	return character >= '0' && character <= '9';
}

bool Scanner::isCapitalLetter(char character) {
	return character >= 'A' && character <= 'Z';
}

bool Scanner::isSmallLetter(char character) {
	return character >= 'a' && character <= 'z';
}

bool Scanner::isVariableCharacter(char character) {
	return character == '_' || isCapitalLetter(character) || isSmallLetter(character);
}

bool Scanner::isHex(char character) {
	return character >= 'A' && character <= 'F' || isNumber(character);
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

std::string getPositionInSourceString(const Token::Position& position) {
	return "at line " + std::to_string(position.lineNumber) + ", at column " + std::to_string(position.columnNumber);
}

bool Scanner::tryBuildValueIdentifier() {
	return true;

}

bool Scanner::tryBuildTypeIdentifier() {
	return true;

}
bool Scanner::tryBuildHexConst() {
	return true;

}
bool Scanner::tryBuildDecimalConst() {
	return true;
}

void Scanner::next() {

	try {
		unsigned long tokenLine = line;
		unsigned long tokenColumn = column;
		unsigned long tokenPosition = position;
		unsigned long currentCharacterCountNumber = 0;

		while (isspace(character)) {
			if (++currentCharacterCountNumber > MAX_EMPTY_SPACE_LENGTH) {
				Token::Position tokenStartPosition{ tokenLine, tokenColumn, tokenPosition };
				std::string errorMessage = "Expected token, got " + std::to_string(currentCharacterCountNumber) + " blank characters " + getPositionInSourceString(tokenStartPosition);
				throw SyntaxError(errorMessage, tokenStartPosition);
			}
			getNextChar();
		}


		currentCharacterCountNumber = 1;

		tokenLine = line;
		tokenColumn = column;
		tokenPosition = position;

		Token::Position tokenStartPosition{ line, column, position };

		if (input.eof()) {
			currentToken = Token(Token::TokenType::END_OF_FILE, "", tokenStartPosition);
			return;
		}

		std::string tokenValue;
		tokenValue += character;

		Token::TokenType tokenType;


		if (isSmallLetter(character)) {
			tokenType = Token::TokenType::VARIABLE_IDENTIFIER;
			while (isVariableCharacter(getNextChar())) {
				if (++currentCharacterCountNumber > MAX_NAME_LENGTH) {
					std::string errorMessage = "Variable name exceeded maximum length " + getPositionInSourceString(tokenStartPosition);
					throw SyntaxError(errorMessage, tokenStartPosition);
				}
				tokenValue += character;
			}
		}
		else if (isCapitalLetter(character)) {
			tokenType = Token::TokenType::TYPE_IDENTIFIER;
			while (isVariableCharacter(getNextChar())) {
				if (++currentCharacterCountNumber > MAX_NAME_LENGTH) {
					std::string errorMessage = "Type name exceeded maximum length " + getPositionInSourceString(tokenStartPosition);
					throw SyntaxError(errorMessage, tokenStartPosition);
				}
				tokenValue += character;
			}
		}
		else if (isNumber(character)) {
			tokenType = Token::TokenType::DECIMAL_CONST;
			while (isNumber(getNextChar())) {
				if (++currentCharacterCountNumber > MAX_DECIMAL_VALUE_LENGTH) {
					std::string errorMessage = "Decimal value exceeded maximum length " + getPositionInSourceString(tokenStartPosition);
					throw SyntaxError(errorMessage, tokenStartPosition);
				}
				tokenValue += character;
			}
			if (character == '.') {
				++currentCharacterCountNumber;
				tokenValue += character;
				while (isNumber(getNextChar())) {
					if (++currentCharacterCountNumber > MAX_DECIMAL_VALUE_LENGTH) {
						std::string errorMessage = "Decimal value exceeded maximum length " + getPositionInSourceString(tokenStartPosition);
						throw SyntaxError(errorMessage, tokenStartPosition);
					}
					tokenValue += character;
				}
				if (tokenValue.back() == '.') {
					tokenStartPosition.columnNumber++;
					std::string errorMessage = "Expected number but got '" + std::string(1, character) + "' " + getPositionInSourceString(tokenStartPosition);
					throw SyntaxError(errorMessage, tokenStartPosition);
				}
			}
		}
		else {
			switch (character) {
			case '}':
				tokenType = Token::TokenType::CLOSING_BRACE;
				break;
			case '{':
				tokenType = Token::TokenType::OPENING_BRACE;
				break;
			case '(':
				tokenType = Token::TokenType::OPENING_BRACKET;
				break;
			case ')':
				tokenType = Token::TokenType::CLOSING_BRACKET;
				break;
			case ':':
				tokenType = Token::TokenType::COLON;
				break;
			case '.':
				tokenType = Token::TokenType::DOT;
				break;
			case ',':
				tokenType = Token::TokenType::COMMA;
				break;
			case '[':
				tokenType = Token::TokenType::OPENING_SQUARE_BRACE;
				break;
			case ']':
				tokenType = Token::TokenType::CLOSING_SQUARE_BRACE;
				break;
			case '+':
				tokenType = Token::TokenType::PLUS;
				break;
			case '-':
				tokenType = Token::TokenType::MINUS;
				break;
			case '/':
				tokenType = Token::TokenType::SLASH;
				break;
			case '*':
				tokenType = Token::TokenType::ASTERISK;
				break;
			case '=':
				tokenType = Token::TokenType::EQUAL_SIGN;
				break;
			case '?':
				tokenType = Token::TokenType::QUESTION_MARK;
				break;
			case '#':
				while (isHex(getNextChar())) {
					if (++currentCharacterCountNumber > MAX_HEX_VALUE_LENGTH) {
						std::string errorMessage = "Hexadecimal value exceeded maximum length " + getPositionInSourceString(tokenStartPosition);
						throw SyntaxError(errorMessage, tokenStartPosition);
					}
					tokenValue += character;
				}
				if (tokenValue.size() > 1)
					tokenType = Token::TokenType::HEX_CONST;
				else {
					std::string errorMessage = "Expected hexadecimal const value, but got '" + std::string(1, character) + "' " + getPositionInSourceString(tokenStartPosition);
					throw SyntaxError(errorMessage, tokenStartPosition);
				}
				break;
			default:
				tokenType = Token::TokenType::UNDEFINED;
				break;
			}
			if (tokenType != Token::TokenType::HEX_CONST)
				getNextChar();
		}

		currentToken = Token(tokenType, tokenValue, tokenStartPosition);
	}
	catch (...) {
		currentToken = Token(Token::TokenType::UNDEFINED, "", Token::Position{});
		throw;
	}
}