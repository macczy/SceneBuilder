#include "pch.h"
#include <deque>
#include <exception>
#include "../SceneBuilder/Parser/Parser.cpp"

class ScannerMock : public Scanner {
public:
	ScannerMock(std::deque<Token> tokens) : tokens(tokens) {
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

TEST(ParserUnitTests, CreateColor) {
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

inline std::deque<Token> getValidPointTokenSequence() {
	return {
			Token(Token::TokenType::TYPE_IDENTIFIER, "Point"),
			Token(Token::TokenType::OPENING_BRACKET, "("),
			Token(Token::TokenType::DECIMAL_CONST, "0.12"),
			Token(Token::TokenType::COMMA, ","),
			Token(Token::TokenType::DECIMAL_CONST, "12.4"),
			Token(Token::TokenType::COMMA, ","),
			Token(Token::TokenType::DECIMAL_CONST, "123.5"),
			Token(Token::TokenType::CLOSING_BRACKET, ")")
	};
}


TEST(ParserUnitTests, CreatePoint) {
	{
		ScannerMock scanner(getValidPointTokenSequence());
		Parser parser(scanner);

		if (auto point = parser.tryBuildPoint(); point.has_value()) {
			auto& [x, y, z] = point->getValues();
			EXPECT_EQ(x.getValue(), "0.12");
			EXPECT_EQ(y.getValue(), "12.4");
			EXPECT_EQ(z.getValue(), "123.5");
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
			auto& [x, y, z] = point->getValues();
			EXPECT_EQ(x.getValue(), "0.12");
			EXPECT_EQ(y.getValue(), "0");
			EXPECT_EQ(z.getValue(), "0");
		}
		else {
			FAIL() << "Expected Point";
		}
	}
	{
		auto tokens = getValidPointTokenSequence();
		tokens.insert(tokens.begin() + 2, Token(Token::TokenType::MINUS, "-"));
		tokens.insert(tokens.begin() + 5, Token(Token::TokenType::MINUS, "-"));
		tokens.insert(tokens.begin() + 8, Token(Token::TokenType::MINUS, "-"));
		ScannerMock scanner(tokens);
		Parser parser(scanner);
		if (auto point = parser.tryBuildPoint(); point.has_value()) {
			auto& [x, y, z] = point->getValues();
			EXPECT_EQ(x.getValue(), "-0.12");
			EXPECT_EQ(y.getValue(), "-12.4");
			EXPECT_EQ(z.getValue(), "-123.5");
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
				FAIL() << "Point building should fail when using hex values";
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
	}
}

TEST(ParserUnitTests, CreateValue) {
	//decimal values through Value
	{
		ScannerMock scanner({
			Token(Token::TokenType::MINUS, "-"),
			Token(Token::TokenType::DECIMAL_CONST, "0.12"),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "test"),
			});
		Parser parser(scanner);

		if (auto value = parser.tryBuildValue(); value.has_value()) {
			EXPECT_EQ(value->index(), Value::DECIMAL_VALUE_INDEX);
			DecimalValue dec = std::get<DecimalValue>(value.value());
			EXPECT_EQ(dec.getValue(), "-0.12");
		}
		else {
			FAIL() << "Expected Decimal value";
		}
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::DECIMAL_CONST, "0.12"),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "test"),
			});
		Parser parser(scanner);

		if (auto value = parser.tryBuildValue(); value.has_value()) {
			EXPECT_EQ(value->index(), Value::DECIMAL_VALUE_INDEX);
			DecimalValue dec = std::get<DecimalValue>(value.value());
			EXPECT_EQ(dec.getValue(), "0.12");
		}
		else {
			FAIL() << "Expected Decimal value";
		}
	}
	//point through Value
	{
		ScannerMock scanner(getValidPointTokenSequence());
		Parser parser(scanner);

		if (auto value = parser.tryBuildValue(); value.has_value()) {
			EXPECT_EQ(value->index(), Value::POINT_INDEX);
			Point point = std::get<Point>(value.value());
			auto& [x, y, z] = point.getValues();
			EXPECT_EQ(x.getValue(), "0.12");
			EXPECT_EQ(y.getValue(), "12.4");
			EXPECT_EQ(z.getValue(), "123.5");
		}
		else {
			FAIL() << "Expected Point";
		}
	}
	//color through value
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

		if (auto value = parser.tryBuildValue(); value.has_value()) {
			EXPECT_EQ(value->index(), Value::COLOR_INDEX);
			Color color = std::get<Color>(value.value());
			EXPECT_EQ(color.getValues(), tripleHexValues("#AB234", "#123", "#0123"));
		}
		else {
			FAIL() << "Expected Color";
		}
	}
}