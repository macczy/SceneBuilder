#include "Token.h"

std::map<Token::TokenType, std::string> Token::TokenTypeToStringMap {
		{ TokenType::UNKNOWN, "UNKNOWN"},
		{ TokenType::OPENING_BRACKET, "OPENING_BRACKET"},
		{ TokenType::CLOSING_BRACKET, "CLOSING_BRACKET"},
		{ TokenType::OPENING_BRACE, "OPENING_BRACE"},
		{ TokenType::CLOSING_BRACE, "CLOSING_BRACE"},
		{ TokenType::OPENING_SQUARE_BRACE, "OPENING_SQUARE_BRACE"},
		{ TokenType::CLOSING_SQUARE_BRACE, "CLOSING_SQUARE_BRACE"},
		{ TokenType::HEX_CONST, "HEX_CONST"},
		{ TokenType::DECIMAL_CONST, "DECIMAL_CONST"},
		{ TokenType::VARIABLE_IDENTIFIER, "VARIABLE_IDENTIFIER"},
		{ TokenType::TYPE_IDENTIFIER, "TYPE_IDENTIFIER"},
		{ TokenType::PLUS, "PLUS"},
		{ TokenType::MINUS, "MINUS"},
		{ TokenType::SLASH, "SLASH"},
		{ TokenType::ASTERISK, "ASTERISK"},
		{ TokenType::END_OF_FILE, "END_OF_FILE"},
		{ TokenType::DOT, "DOT"},
		{ TokenType::COMMA, "COMMA"},
		{ TokenType::COLON, "COLON"},
		{ TokenType::EQUAL_SIGN, "EQUAL_SIGN"}
};

std::ostream& operator<< (std::ostream& stream, const Token& token) {
	stream << "Token " << token.type;
	return stream;
}


std::ostream& operator<< (std::ostream& stream, const Token::TokenType& tokenType) {
	stream << Token::TokenTypeToStringMap[tokenType];
	return stream;
}