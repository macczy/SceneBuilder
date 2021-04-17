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
	return character == '_' || isCapitalLetter(character) || isSmallLetter(character);
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
		currentToken = Token(Token::TokenType::DECIMAL_CONST, tokenValue, tokenStartPosition);
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
	input.seekg(0);
	for (unsigned int currLineNumber = 0; currLineNumber < position.lineNumber; ++currLineNumber) {
		if (!input.ignore(std::numeric_limits<std::streamsize>::max(), input.widen('\n'))) {
			//throw std::runtime_error("Number of lines in source is less than " + std::to_string(position.lineNumber)); //if there is no line with this line number, throw error
		}
	}
	std::getline(input, buffer);
	std::replace(buffer.begin(), buffer.end(), '\t', ' ');
	buffer = "\n" + buffer + "\n" + std::string(position.columnNumber - 1, ' ') + "^";
	input.seekg(positionInInput);
	return buffer;
}

bool Scanner::isDecimalConst(Token::Position tokenStartPosition) {
	if (isDigit(character)) {
		std::string tokenValue(1, character);
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
			if (tokenValue.back() == '.') { //number ends with a dot, which I see as incorrect
				tokenStartPosition.columnNumber += (unsigned int)tokenValue.size()-1;
				std::string errorMessage = "Expected number but got '" + std::string(1, character) + "' " 
					+ tokenStartPosition.toString() + getLineError(tokenStartPosition);
				throw SyntaxError(errorMessage);
			}
		}
		currentToken = Token(Token::TokenType::DECIMAL_CONST, tokenValue, tokenStartPosition);
		return true;
	}
	return false;
}

void Scanner::next() {
	try {
		unsigned long tokenLine = line;
		unsigned long tokenColumn = column;
		unsigned long tokenPosition = position;
		unsigned long currentCharacterCountNumber = 0;

		while (isspace(character)) {
			checkIfTokenMaxLengthReached(MAX_EMPTY_SPACE_LENGTH, Token::Position{ tokenLine, tokenColumn, tokenPosition }, 
				"Expected token, got " + std::to_string(currentCharacterCountNumber) + " blank characters ", ++currentCharacterCountNumber);
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

		std::string tokenValue(1, character);



		if (isVariableIdentifier(tokenStartPosition)) return;

		if (isTypeIdentifier(tokenStartPosition)) return;

		if (isDecimalConst(tokenStartPosition)) return;

		if (isHexConst(tokenStartPosition)) return;
		if (character == '|') {
			if (getNextChar() == '|') {
				tokenValue += character;
				currentToken = Token(Token::TokenType::OR, tokenValue, tokenStartPosition);
				getNextChar();
			}
			else {
				tokenStartPosition.columnNumber++;
				std::string errorMessage = "Expected | but got '" + std::string(1, character) + "' " + tokenStartPosition.toString();
				throw SyntaxError(errorMessage);
			}
		} else if (character == '&') {
			if (getNextChar() == '&') {
				tokenValue += character;
				currentToken = Token(Token::TokenType::AND, tokenValue, tokenStartPosition);
				getNextChar();
			}
			else {
				tokenStartPosition.columnNumber++;
				std::string errorMessage = "Expected & but got '" + std::string(1, character) + "' " + tokenStartPosition.toString();
				throw SyntaxError(errorMessage);
			}
		} else if (character == '<' ) {
			if (getNextChar() == '=') {
				tokenValue += character;
				currentToken = Token(Token::TokenType::LESS_OR_EQUAL, tokenValue, tokenStartPosition);
				getNextChar();
			}
			else
				currentToken = Token(Token::TokenType::LESS_THAN, tokenValue, tokenStartPosition);
		}
		else if (character == '>' ) {
			if (getNextChar() == '=') {
				tokenValue += character;
				currentToken = Token(Token::TokenType::GREATER_OR_EQUAL, tokenValue, tokenStartPosition);
				getNextChar();
			}
			else
				currentToken = Token(Token::TokenType::GREATER_THAN, tokenValue, tokenStartPosition);
		} else {
			switch (character) {
			case '}':
				currentToken = Token(Token::TokenType::CLOSING_BRACE, tokenValue, tokenStartPosition);
				break;
			case '{':
				currentToken = Token(Token::TokenType::OPENING_BRACE, tokenValue, tokenStartPosition);
				break;
			case '(':
				currentToken = Token(Token::TokenType::OPENING_BRACKET, tokenValue, tokenStartPosition);
				break;
			case ')':
				currentToken = Token(Token::TokenType::CLOSING_BRACKET, tokenValue, tokenStartPosition);
				break;
			case ':':
				currentToken = Token(Token::TokenType::COLON, tokenValue, tokenStartPosition);
				break;
			case '.':
				currentToken = Token(Token::TokenType::DOT, tokenValue, tokenStartPosition);
				break;
			case ',':
				currentToken = Token(Token::TokenType::COMMA, tokenValue, tokenStartPosition);
				break;
			case '[':
				currentToken = Token(Token::TokenType::OPENING_SQUARE_BRACE, tokenValue, tokenStartPosition);
				break;
			case ']':
				currentToken = Token(Token::TokenType::CLOSING_SQUARE_BRACE, tokenValue, tokenStartPosition);
				break;
			case '+':
				currentToken = Token(Token::TokenType::PLUS, tokenValue, tokenStartPosition);
				break;
			case '-':
				currentToken = Token(Token::TokenType::MINUS, tokenValue, tokenStartPosition);
				break;
			case '/':
				currentToken = Token(Token::TokenType::SLASH, tokenValue, tokenStartPosition);
				break;
			case '*':
				currentToken = Token(Token::TokenType::ASTERISK, tokenValue, tokenStartPosition);
				break;
			case '=':
				currentToken = Token(Token::TokenType::EQUAL_SIGN, tokenValue, tokenStartPosition);
				break;
			case '?':
				currentToken = Token(Token::TokenType::QUESTION_MARK, tokenValue, tokenStartPosition);
				break;
			default:
				currentToken = Token(Token::TokenType::UNDEFINED, tokenValue, tokenStartPosition);
				break;
			}
			getNextChar();
		}
	}
	catch (...) {
		currentToken = Token(Token::TokenType::UNDEFINED, "", Token::Position{});
		throw;
	}
}