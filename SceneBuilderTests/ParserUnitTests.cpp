#include "pch.h"
#include <deque>
#include <exception>
#include "../SceneBuilder/Parser/Parser.cpp"
#include "../SceneBuilder/Objects/Value.cpp"
#include "../SceneBuilder/Objects/Comparison.cpp"

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

TEST(ParserUnitTests, CreateIdentifier) {
	ScannerMock scanner({
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "house"),
		Token(TokenType::END_OF_FILE)
		});
	Parser parser(scanner);

	if (auto identifier = parser.tryBuildIdentifier(); identifier.has_value()) {
		EXPECT_EQ(identifier->getValue(), "house");
		EXPECT_EQ(identifier->hasNext(), false);
	}
	else {
		FAIL() << "Expected identifier";
	}
}
TEST(ParserUnitTests, CreateIdentifierPropertyList) {
	ScannerMock scanner({
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "house"),
		Token(Token::TokenType::DOT, "."),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "roof"),
		Token(Token::TokenType::DOT, "."),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "chimeny"),
		Token(TokenType::END_OF_FILE)
		});
	Parser parser(scanner);
	if (auto identifier = parser.tryBuildIdentifier(); identifier.has_value()) {
		EXPECT_EQ(identifier->getValue(), "house");
		EXPECT_EQ(identifier->hasNext(), true);
		auto roof = identifier->getNext();
		EXPECT_EQ(roof->getValue(), "roof");
		EXPECT_EQ(roof->hasNext(), true);
		auto house = roof->getNext();
		EXPECT_EQ(house->getValue(), "chimeny");
		EXPECT_EQ(house->hasNext(), false);
	}
	else {
		FAIL() << "Expected identifier";
	}
}
TEST(ParserUnitTests, FailCreateIdentifier) {
	{
		ScannerMock scanner({
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "roof"),
			Token(Token::TokenType::DOT, "."),
			Token(TokenType::END_OF_FILE)
			});
		Parser parser(scanner);
		EXPECT_THROW({
			if (auto identifier = parser.tryBuildIdentifier(); identifier.has_value()) {
				FAIL() << "Identifier building should fail";
			}
			}, SyntaxError);
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "roof"),
			Token(Token::TokenType::DOT, "."),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "house"),
			Token(Token::TokenType::DOT, "."),
			Token(TokenType::END_OF_FILE)
			});
		Parser parser(scanner);
		EXPECT_THROW({
			if (auto identifier = parser.tryBuildIdentifier(); identifier.has_value()) {
				FAIL() << "Identifier building should fail";
			}
			}, SyntaxError);
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
			EXPECT_TRUE(std::holds_alternative<DecimalValue>(value.value()));
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
			EXPECT_TRUE(std::holds_alternative<DecimalValue>(value.value()));
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
			EXPECT_TRUE(std::holds_alternative<Point>(value.value()));
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
			EXPECT_TRUE(std::holds_alternative<Color>(value.value()));
			Color color = std::get<Color>(value.value());
			EXPECT_EQ(color.getValues(), tripleHexValues("#AB234", "#123", "#0123"));
		}
		else {
			FAIL() << "Expected Color";
		}
	}
}


TEST(ParserUnitTests, CreateAdditionPlus) {
	ScannerMock scanner({
		Token(Token::TokenType::PLUS, "+"),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "house"),
		Token(Token::TokenType::DOT, "."),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "width"),
		Token(TokenType::END_OF_FILE)
		});
	Parser parser(scanner);

	Value val = DecimalValue(Position(), "12.4");
	if (auto addition = parser.tryBuildAddition(val); addition.has_value()) {
		EXPECT_EQ(addition->getOperator(), Addition::Operator::PLUS);

		EXPECT_TRUE(std::holds_alternative<Identifier>(*addition->getSecondValue()));

		EXPECT_EQ(std::get<Identifier>(*addition->getSecondValue()).getValue(), "house");

		EXPECT_TRUE(std::holds_alternative<DecimalValue>(*addition->getFirstValue()));

		EXPECT_EQ(std::get<DecimalValue>(*addition->getFirstValue()).getValue(), "12.4");
	}
	else {
		FAIL() << "Expected addition operation";
	}
}

TEST(ParserUnitTests, CreateAdditionMinus) {
	ScannerMock scanner({
		Token(Token::TokenType::MINUS, "-"),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "width"),
		Token(TokenType::END_OF_FILE)
		});
	Parser parser(scanner);
	Value val = DecimalValue(Position(), "12.4");
	if (auto addition = parser.tryBuildAddition(val); addition.has_value()) {
		EXPECT_EQ(addition->getOperator(), Addition::Operator::MINUS);
	}
	else {
		FAIL() << "Expected identifier";
	}
}


TEST(ParserUnitTests, CreateLogicalExpression) {
	//ScannerMock scanner({
	//	Token(Token::TokenType::MINUS, "-"),
	//	Token(Token::TokenType::VARIABLE_IDENTIFIER, "width"),
	//	Token(TokenType::END_OF_FILE)
	//	});
	//Parser parser(scanner);
	//Value val = DecimalValue(Position(), "12.4");
	//if (auto addition = parser.tryBuildAddition(val); addition.has_value()) {
	//	EXPECT_EQ(addition->getOperator(), Addition::Operator::MINUS);
	//}
	//else {
	FAIL() << "Expected LogicalExpression";
	//}
}


TEST(ParserUnitTests, CreateComparison) {
	{
		ScannerMock scanner({
			Token(Token::TokenType::EQUAL_SIGN, "="),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "width"),
			Token(TokenType::END_OF_FILE)
			});
		Parser parser(scanner);
		Value val = DecimalValue(Position(), "12.4");
		if (auto comparison = parser.tryBuildComparison(val); comparison.has_value()) {
			//EXPECT_EQ(comparison->getOperator(), Comparison::Operator::EQUAL);
		}
		else
			FAIL() << "Expected equality comparison";
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::NOT_EQUAL, "!="),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "width"),
			Token(TokenType::END_OF_FILE)
			});
		Parser parser(scanner);
		Value val = DecimalValue(Position(), "12.4");
		if (auto comparison = parser.tryBuildComparison(val); comparison.has_value()) {
			//EXPECT_EQ(comparison->getOperator(), Comparison::Operator::NOT_EQUAL);
		}
		else
			FAIL() << "Expected equality comparison";
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::EQUAL_SIGN, "="),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "width"),
			Token(TokenType::END_OF_FILE)
			});
		Parser parser(scanner);
		Value val = DecimalValue(Position(), "12.4");
		if (auto comparison = parser.tryBuildComparison(val); comparison.has_value()) {
			//EXPECT_EQ(comparison->getOperator(), Comparison::Operator::EQUAL);
		}
		else
			FAIL() << "Expected equality comparison";
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::EQUAL_SIGN, "="),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "width"),
			Token(TokenType::END_OF_FILE)
			});
		Parser parser(scanner);
		Value val = DecimalValue(Position(), "12.4");
		if (auto comparison = parser.tryBuildComparison(val); comparison.has_value()) {
			//EXPECT_EQ(comparison->getOperator(), Comparison::Operator::EQUAL);
		}
		else
			FAIL() << "Expected equality comparison";
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::EQUAL_SIGN, "="),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "width"),
			Token(TokenType::END_OF_FILE)
			});
		Parser parser(scanner);
		Value val = DecimalValue(Position(), "12.4");
		if (auto comparison = parser.tryBuildComparison(val); comparison.has_value()) {
			//EXPECT_EQ(comparison->getOperator(), Comparison::Operator::EQUAL);
		}
		else
			FAIL() << "Expected equality comparison";
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::EQUAL_SIGN, "="),
			Token(Token::TokenType::EQUAL_SIGN, "+"),
			Token(TokenType::END_OF_FILE)
			});
		Parser parser(scanner);
		EXPECT_THROW({
		Value val = DecimalValue(Position(), "12.4");
			if (auto comparison = parser.tryBuildComparison(val); comparison.has_value()) {
				FAIL() << "Comparison building should throw";
			}
			}, SyntaxError);
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "fcdsad"),
			Token(TokenType::END_OF_FILE)
			});
		Parser parser(scanner);
		Value val = DecimalValue(Position(), "12.4");
		if (auto comparison = parser.tryBuildComparison(val); comparison.has_value()) {
			FAIL() << "Comparison building should return nullopt";
		}
	}
}

