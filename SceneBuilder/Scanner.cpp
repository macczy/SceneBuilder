#include <string>  
#include "Scanner.h"

Scanner::Scanner(std::istream& input) : input(input), line(0), position(0) {
	
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
	char character = input.get();
	++position;
	if (character == '\n') {
		++line;
		position = 0;
	}
	return character;
}

void Scanner::next() {
	char character;

	while (isspace(character = getNextChar()));

	long tokenStartLine = line;
	long tokenStartPosition = position;

	if (input.eof()) {
		currentToken = Token(Token::TokenType::END_OF_FILE, "", tokenStartLine, tokenStartPosition);
		return;
	}

	std::string tokenValue;
	tokenValue += character;

	Token::TokenType tokenType;

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
	case '"':
		tokenType = Token::TokenType::QUOTATION_MARK;
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
	default:
		if (isNumber(character)) {
			tokenType = Token::TokenType::DECIMAL_CONST;
			while (isNumber(character = input.peek())) {
				tokenValue += getNextChar();
			}
			if (character == '.') {
				tokenValue += getNextChar();
				while (isNumber(character = input.peek())) {
					tokenValue += getNextChar();
				}
				char last_character = tokenValue[tokenValue.size() - 1];
				if (last_character == '.') {
					throw std::runtime_error("Expected number but got '" + std::string(1, input.peek()) + "' at line " + std::to_string(line) + ", position " + std::to_string(position));
				}
			}
		} else if (isCapitalLetter(character)) {
			tokenType = Token::TokenType::TYPE_IDENTIFIER;
			while (isVariableCharacter(character = input.peek())) {
				tokenValue += getNextChar();
			}

		}
		else if(isSmallLetter(character)) {
			if (character == 'x') {
				while (isHex(character = input.peek())) {
					tokenValue += getNextChar();
				}
				if (tokenValue.size() > 1 && !isVariableCharacter(character = input.peek())) {
					tokenType = Token::TokenType::HEX_CONST;
					break;
				}
			}
			tokenType = Token::TokenType::VARIABLE_IDENTIFIER;
			while (isVariableCharacter(character = input.peek())) {
				tokenValue += getNextChar();
			}
		}
		else {
			tokenType = Token::TokenType::UNKNOWN;
		}
	}

	currentToken = Token(tokenType, tokenValue, tokenStartLine, tokenStartPosition);
}