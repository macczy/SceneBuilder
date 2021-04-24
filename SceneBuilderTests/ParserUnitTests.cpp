#include "pch.h"
#include <deque>
#include <exception>
#include "../SceneBuilder/Parser/Parser.cpp"

class ScannerMock : public Scanner {
public:
	ScannerMock(std::deque<Token> tokens, std::stringstream& str = std::stringstream("")) : tokens(tokens) {
		next();
	}
	Token virtual getToken() {
		return _currentToken;
	}
	void virtual next() {
		if (tokens.empty()) {
			throw std::runtime_error("no next token");
		}

		_currentToken = tokens.front();
		tokens.pop_front();
	}
private:
	std::deque<Token> tokens;
	Token _currentToken;
};
//Ignoring position part of tokens for these tests, as this does not concern us here

TEST(ParserUnitTests, ConstructorTest) {
	try {
		ScannerMock scanner({ Token(Token::TokenType::UNDEFINED, "") });
		Parser parser(scanner);
		std::unique_ptr<SceneRoot> root = parser.parse();
	}
	catch (const SyntaxError& err) {
		std::cout << err.what();
	}
	catch (const std::runtime_error& err) {
		std::cout << err.what();
	}
}

TEST(ParserUnitTests, CreateColorObject) {
	{
		ScannerMock scanner({
			Token(Token::TokenType::TYPE_IDENTIFIER, "Color"),
			Token(Token::TokenType::OPENING_BRACKET, "("),
			Token(Token::TokenType::HEX_CONST, "#AB234"),
			Token(Token::TokenType::COMMA, ","),
			Token(Token::TokenType::HEX_CONST, "#123"),
			Token(Token::TokenType::COMMA, ","),
			Token(Token::TokenType::HEX_CONST, "#0123"),
			Token(Token::TokenType::CLOSING_BRACKET, ")")
			});
		Parser parser(scanner);
		
		if (auto color = parser.tryBuildColor(); color.has_value()) {
			EXPECT_EQ(color->getValues(), tripleHexValues("#AB234", "#123", "#0123"));
		} else {
			FAIL() << "Expected Color";
		}
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::TYPE_IDENTIFIER, "Color"),
			Token(Token::TokenType::OPENING_BRACKET, "("),
			Token(Token::TokenType::DECIMAL_CONST, "123"),
			Token(Token::TokenType::COMMA, ","),
			Token(Token::TokenType::HEX_CONST, "#AB234"),
			Token(Token::TokenType::COMMA, ","),
			Token(Token::TokenType::HEX_CONST, "#AB234"),
			Token(Token::TokenType::CLOSING_BRACE, ")")
			});
		Parser parser(scanner);
		EXPECT_THROW({
			if (auto color = parser.tryBuildColor(); color) {
				FAIL() << "Color building should fail";
			}
		}, SyntaxError);
	}
}


TEST(ParserUnitTests, CreatePointObject) {
	{
		ScannerMock scanner({
			Token(Token::TokenType::TYPE_IDENTIFIER, "Point"),
			Token(Token::TokenType::OPENING_BRACKET, "("),
			Token(Token::TokenType::DECIMAL_CONST, "0.12"),
			Token(Token::TokenType::COMMA, ","),
			Token(Token::TokenType::DECIMAL_CONST, "12.4"),
			Token(Token::TokenType::COMMA, ","),
			Token(Token::TokenType::DECIMAL_CONST, "123.5"),
			Token(Token::TokenType::CLOSING_BRACKET, ")")
			});
		Parser parser(scanner);

		if (auto point = parser.tryBuildPoint(); point.has_value()) {
			EXPECT_EQ(point->getValues(), triplePointValues("0.12", "12.4", "123.5"));
		}
		else {
			FAIL() << "Expected Point";
		}
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::TYPE_IDENTIFIER, "Point"),
			Token(Token::TokenType::OPENING_BRACKET, "("),
			Token(Token::TokenType::DECIMAL_CONST, "0.12"),
			Token(Token::TokenType::CLOSING_BRACKET, ")")
			});
		Parser parser(scanner);

		if (auto point = parser.tryBuildPoint(); point.has_value()) {
			EXPECT_EQ(point->getValues(), triplePointValues("0.12", "0", "0"));
		}
		else {
			FAIL() << "Expected Point";
		}
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::TYPE_IDENTIFIER, "Point"),
			Token(Token::TokenType::OPENING_BRACKET, "("),
			Token(Token::TokenType::MINUS, "-"),
			Token(Token::TokenType::DECIMAL_CONST, "0.12"),
			Token(Token::TokenType::COMMA, ","),
			Token(Token::TokenType::MINUS, "-"),
			Token(Token::TokenType::DECIMAL_CONST, "12.4"),
			Token(Token::TokenType::COMMA, ","),
			Token(Token::TokenType::MINUS, "-"),
			Token(Token::TokenType::DECIMAL_CONST, "123.5"),
			Token(Token::TokenType::CLOSING_BRACKET, ")")
			});
		Parser parser(scanner);

		if (auto point = parser.tryBuildPoint(); point.has_value()) {
			EXPECT_EQ(point->getValues(), triplePointValues("-0.12", "-12.4", "-123.5"));
		}
		else {
			FAIL() << "Expected Point";
		}
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::TYPE_IDENTIFIER, "Point"),
			Token(Token::TokenType::OPENING_BRACKET, "("),
			Token(Token::TokenType::DECIMAL_CONST, "123"),
			Token(Token::TokenType::COMMA, ","),
			Token(Token::TokenType::HEX_CONST, "#AB234"),
			Token(Token::TokenType::COMMA, ","),
			Token(Token::TokenType::HEX_CONST, "#AB234"),
			Token(Token::TokenType::CLOSING_BRACE, ")")
			});
		Parser parser(scanner);
		EXPECT_THROW({
			if (auto point = parser.tryBuildPoint(); point) {
				FAIL() << "Point building should fail";
			}
		}, SyntaxError);
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::TYPE_IDENTIFIER, "Color"),
			});
		Parser parser(scanner);

		if (auto point = parser.tryBuildPoint(); point.has_value()) {
			FAIL() << "Should return nullopt";
		}
		else {
		}
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "Point"),
			});
		Parser parser(scanner);

		if (auto point = parser.tryBuildPoint(); point.has_value()) {
			FAIL() << "Should return nullopt";
		}
		else {
		}
	}
}
