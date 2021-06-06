#include <string>  
#include <algorithm>
#include "Scanner.h"
#include "../Exceptions/TokenTooLong.h"
#include "../Exceptions/SyntaxError.h"

SceneBuilderScanner::SceneBuilderScanner(std::istream& input) : input(input), line(1), column(0) {
	singleCharTokens = { 
		{'}', TokenType::CLOSING_BRACE},
		{'{', TokenType::OPENING_BRACE},
		{'(', TokenType::OPENING_BRACKET},
		{')', TokenType::CLOSING_BRACKET},
		{':', TokenType::COLON},
		{'.', TokenType::DOT},
		{',', TokenType::COMMA},
		{'[', TokenType::OPENING_SQUARE_BRACE},
		{']', TokenType::CLOSING_SQUARE_BRACE},
		{'+', TokenType::PLUS},
		{'-', TokenType::MINUS},
		{'/', TokenType::SLASH},
		{'*', TokenType::ASTERISK},
		{'=', TokenType::EQUAL_SIGN},
		{'?', TokenType::QUESTION_MARK},
		{'"', TokenType::DOUBLE_QUOTE_CHARACTER}
	};
	lambdaGeneratedTokens = {
		{'|', [=](Position tokenStartPosition) {
			if (getNextChar() == '|') {
				currentToken = Token(TokenType::OR, "||", tokenStartPosition);
				getNextChar();
			}
			else {
				tokenStartPosition.columnNumber++;
				throw SyntaxError("|", std::string(1, character), tokenStartPosition);
				}}},
		{'&', [=](Position tokenStartPosition) {
			if (getNextChar() == '&') {
				currentToken = Token(TokenType::AND, "&&", tokenStartPosition);
				getNextChar();
			}
			else {
				tokenStartPosition.columnNumber++;
				throw SyntaxError("&", std::string(1, character), tokenStartPosition);
			}}},
		{'<', [=](Position tokenStartPosition) {
			if (getNextChar() == '=') {
				currentToken = Token(TokenType::LESS_OR_EQUAL, "<=", tokenStartPosition);
				getNextChar();
			}
			else
				currentToken = Token(TokenType::LESS_THAN, "<", tokenStartPosition);
			}},
		{'>', [=](Position tokenStartPosition) {
			if (getNextChar() == '=') {
				currentToken = Token(TokenType::GREATER_OR_EQUAL, ">=", tokenStartPosition);
				getNextChar();
			}
			else
				currentToken = Token(TokenType::GREATER_THAN, ">", tokenStartPosition);
			}},
		{'!', [=](Position tokenStartPosition) {
			if (getNextChar() == '=') {
				currentToken = Token(TokenType::NOT_EQUAL, "!=", tokenStartPosition);
				getNextChar();
			}
			else
				currentToken = Token(TokenType::UNDEFINED, "!", tokenStartPosition);
			}}
	};
	keyWords = {
		{ "Wait", TokenType::WAIT_KEYWORD},
		{ "Animation", TokenType::ANIMATION_KEYWORD},
		{ "AnimationSequence", TokenType::ANIMATION_SEQUENCE_KEYWORD},
		{ "ParalelAnimation", TokenType::PARALEL_ANIMATION_KEYWORD},
		{ "ConditionalAnimation",TokenType::CONDITIONAL_ANIMATION_KEYWORD},
		{ "Line",TokenType::LINE_KEYWORD},
		{ "Circle",TokenType::CIRCLE_KEYWORD},
		{ "Rectangle",TokenType::RECTANGLE_KEYWORD},
		{ "Polygon",TokenType::POLYGON_KEYWORD},
		{ "Scene",TokenType::SCENE_KEYWORD}//,
		//{ "condition", TokenType::CONDITION_KEYWORD}
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
		throw TokenTooLong(errorMessage, tokenStartPosition);
	}
}

bool SceneBuilderScanner::isVariableIdentifier(Position tokenStartPosition) {
	if (isSmallLetter(character)) {
		std::string tokenValue(1, character);
		while (isVariableCharacter(getNextChar())) {
			tokenValue += character;
			checkIfTokenMaxLengthReached(MAX_NAME_LENGTH, tokenStartPosition, "Variable name", tokenValue.size());
		}
		auto keyword = keyWords.find(tokenValue);
		if (keyword != keyWords.end())
			currentToken = Token(keyword->second, tokenValue, tokenStartPosition);
		else
			currentToken = Token(TokenType::VARIABLE_IDENTIFIER, tokenValue, tokenStartPosition);
		return true;
	}
	return false;
}

bool SceneBuilderScanner::isTypeIdentifier(Position tokenStartPosition) {
	if (isCapitalLetter(character)) {
		std::string tokenValue(1, character);
		while (isVariableCharacter(getNextChar())) {
			tokenValue += character;
			checkIfTokenMaxLengthReached(MAX_NAME_LENGTH, tokenStartPosition, "Type name", tokenValue.size());
		}		
		auto keyword = keyWords.find(tokenValue);
		if (keyword != keyWords.end())
			currentToken = Token(keyword->second, tokenValue, tokenStartPosition);
		else
		currentToken = Token(TokenType::TYPE_IDENTIFIER, tokenValue, tokenStartPosition);
		return true;
	}
	return false;
}

bool SceneBuilderScanner::isHexConst(Position tokenStartPosition) {
	if (character == '#') {
		std::string tokenValue(1, character);
		while (isHex(getNextChar())) {
			tokenValue += character;
			checkIfTokenMaxLengthReached(MAX_HEX_VALUE_LENGTH, tokenStartPosition, "Hexadecimal value", tokenValue.size());
		}
		if (tokenValue.size() == 1) {
			tokenStartPosition.columnNumber++;
			throw SyntaxError("hexadecimal const value", std::string(1, character), tokenStartPosition);
		}
		currentToken = Token(TokenType::HEX_CONST, tokenValue.replace(0, 1, "0x"), tokenStartPosition);
		return true;
	}
	return false;
}

bool SceneBuilderScanner::isDecimalConst(Position tokenStartPosition) {
	if (isDigit(character)) {
		std::string tokenValue(1, character);
		if (character == 0) { 
			if (getNextChar() == '.') {
				tokenValue += character;
				while (isDigit(getNextChar())) {
					tokenValue += character;
					checkIfTokenMaxLengthReached(MAX_DECIMAL_VALUE_LENGTH, tokenStartPosition, "Decimal value", tokenValue.size());
				}
			}
			else {
				currentToken = Token(TokenType::DECIMAL_CONST, tokenValue, tokenStartPosition);
				getNextChar();
			}
		}
		else {
			while (isDigit(getNextChar())) {
				tokenValue += character;
				checkIfTokenMaxLengthReached(MAX_DECIMAL_VALUE_LENGTH, tokenStartPosition, "Decimal value", tokenValue.size());
			}
			if (character == '.') {
				tokenValue += character;
				while (isDigit(getNextChar())) {
					tokenValue += character;
					checkIfTokenMaxLengthReached(MAX_DECIMAL_VALUE_LENGTH, tokenStartPosition, "Decimal value", tokenValue.size());
				}
			}
			currentToken = Token(TokenType::DECIMAL_CONST, tokenValue, tokenStartPosition);
			return true;
		}
	}
	return false;
}

void SceneBuilderScanner::next() {
	try {
		int tokenLine = line;
		int tokenColumn = column;
		std::streamoff tokenPosition = input.tellg();
		Position tokenStartPosition{ tokenLine, tokenColumn, tokenPosition };
		
		unsigned long currentCharacterCountNumber = 0;
		while (isspace(character)) {
			checkIfTokenMaxLengthReached(MAX_EMPTY_SPACE_LENGTH, tokenStartPosition,
				"Blank characters", ++currentCharacterCountNumber);
			getNextChar();
		}
		tokenStartPosition = Position{ line, column, input.tellg() };
		if (input.eof()) {
			currentToken = Token(TokenType::END_OF_FILE, "", tokenStartPosition);
			return;
		}
		if (isVariableIdentifier(tokenStartPosition)) return;
		if (isTypeIdentifier(tokenStartPosition)) return;
		if (isDecimalConst(tokenStartPosition)) return;
		if (isHexConst(tokenStartPosition)) return;

		auto lambda = lambdaGeneratedTokens.find(character);
		if(lambda != lambdaGeneratedTokens.end()) {
			lambda->second(tokenStartPosition);
		} else {
			auto type = singleCharTokens.find(character);
			if (type != singleCharTokens.end())
				currentToken = Token(type->second, std::string(1, character), tokenStartPosition);
			else 
				currentToken = Token(TokenType::UNDEFINED, std::string(1, character), tokenStartPosition);
			getNextChar();
		}
	}
	catch (SyntaxError&) {
		currentToken = Token(TokenType::UNDEFINED, "", Position{ line, column, input.tellg() });
		throw;
	}
}