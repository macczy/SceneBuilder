#pragma once
#include <ostream>
#include <map>
 

struct Position {
	unsigned long lineNumber;
	unsigned long columnNumber;
	std::streamoff totalPositionNumber;
	std::string toString() const;
};

class Token
{
public:
	enum class TokenType {
		UNDEFINED, //default value
		OPENING_BRACKET, 
		CLOSING_BRACKET,
		OPENING_BRACE, 
		CLOSING_BRACE,
		OPENING_SQUARE_BRACE,
		CLOSING_SQUARE_BRACE,
		HEX_CONST,
		DECIMAL_CONST,
		VARIABLE_IDENTIFIER,
		TYPE_IDENTIFIER,
		PLUS,
		MINUS,
		SLASH,
		ASTERISK,
		END_OF_FILE,
		DOT,
		COMMA,
		COLON,
		EQUAL_SIGN,
		QUESTION_MARK,
		LESS_THAN, 
		GREATER_THAN,
		LESS_OR_EQUAL,
		GREATER_OR_EQUAL,
		NOT_EQUAL,
		OR,
		AND
	};

	static std::map<TokenType, std::string> TokenTypeToStringMap;

	const TokenType getType() const {
		return type;
	}
	const std::string getValue() const {
		return value; 
	}

	Position getPosition() {
		return position;
	}

	Token(TokenType type = TokenType::UNDEFINED, std::string value = "", Position position = Position{ 0, 0, 0 }) : type(type), value(value), position(position) {}

	friend std::ostream& operator<< (std::ostream& stream, const Token& token);
	friend std::ostream& operator<< (std::ostream& stream, const TokenType& tokenType);

private:
	std::string value;
	TokenType type;
	Position position;
};


