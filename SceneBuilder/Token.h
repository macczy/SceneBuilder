#pragma once
#include <ostream>
#include <map>
 
class Token
{
public:
	enum class TokenType {
		UNDEFINED,
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

	struct Position {
		unsigned long lineNumber;
		unsigned long columnNumber;
		unsigned long totalPositionNumber;
		std::string toString() const;
	};

	static std::map<TokenType, std::string> TokenTypeToStringMap;

	TokenType getType() {
		return type;
	}
	std::string getValue() {
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


