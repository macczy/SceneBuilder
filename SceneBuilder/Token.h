#pragma once
#include <ostream>
#include <map>
 
class Token
{
public:
	enum class TokenType {
		UNKNOWN,
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
		QUOTATION_MARK
	};

	static std::map<TokenType, std::string> TokenTypeToStringMap;

	TokenType getType() {
		return type;
	}
	std::string getValue() {
		return value; 
	}

	unsigned long getLine() {
		return line;
	}

	unsigned long getPosition() {
		return position;
	}

	Token() : type(TokenType::UNKNOWN), line(0), position(0) {}
	Token(TokenType type, std::string value, unsigned long line, unsigned long position) : type(type), value(value), line(line), position(position) {}

	friend std::ostream& operator<< (std::ostream& stream, const Token& token);
	friend std::ostream& operator<< (std::ostream& stream, const TokenType& tokenType);

private:
	std::string value;
	TokenType type;
	unsigned long line;
	unsigned long position;

};

