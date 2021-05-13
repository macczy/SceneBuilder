#include "pch.h"
#include <deque>
#include <exception>
#include "../SceneBuilder/Parser/Parser.cpp"
#include "../SceneBuilder/Objects/Expression.cpp"
#include "../SceneBuilder/Objects/Comparison.cpp"
#include "../SceneBuilder/Objects/Addition.cpp"
#include "../SceneBuilder/Objects/Multiplication.cpp"
#include "../SceneBuilder/Objects/LogicalExpression.cpp"
#include "../SceneBuilder/Objects/BasicObject.cpp"

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
			_currentToken = Token(TokenType::END_OF_FILE);
			return;
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
		
		if (auto color = parser.tryBuildColor(); color) {
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

		if (auto point = parser.tryBuildPoint(); point) {
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

		if (auto point = parser.tryBuildPoint(); point) {
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
		if (auto point = parser.tryBuildPoint(); point) {
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

		if (auto point = parser.tryBuildPoint(); point) {
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

		if (auto point = parser.tryBuildPoint(); point) {
			FAIL() << "Should return nullopt";
		}
	}
}

TEST(ParserUnitTests, CreateIdentifier) {
	ScannerMock scanner({
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "house")
		});
	Parser parser(scanner);

	if (auto identifier = parser.tryBuildIdentifier(); identifier) {
		EXPECT_EQ(identifier->getValue(), "house");
		EXPECT_EQ(identifier->hasNext(), false);
	}
	else {
		FAIL() << "Expected identifier";
	}
}
TEST(ParserUnitTests, CreateIdentifierPropertyList) {
	{
		ScannerMock scanner({
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "house"),
			Token(Token::TokenType::DOT, "."),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "roof"),
			Token(Token::TokenType::DOT, "."),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "chimeny")
			});
		Parser parser(scanner);
		if (auto identifier = parser.tryBuildIdentifier(); identifier) {
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
	{
		ScannerMock scanner({
	Token(Token::TokenType::VARIABLE_IDENTIFIER, "house"),
	Token(Token::TokenType::DOT, "."),
	Token(Token::TokenType::VARIABLE_IDENTIFIER, "roof")
			});
		Parser parser(scanner);
		if (auto identifier = parser.tryBuildIdentifier(); identifier) {
			EXPECT_EQ(identifier->getValue(), "house");
			EXPECT_EQ(identifier->hasNext(), true);
			auto roof = identifier->getNext();
			EXPECT_EQ(roof->getValue(), "roof");
			EXPECT_EQ(roof->hasNext(), false);
		}
		else {
			FAIL() << "Expected identifier";
		}
	}
}

TEST(ParserUnitTests, FailCreateIdentifier) {
	{
		ScannerMock scanner({
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "roof"),
			Token(Token::TokenType::DOT, ".")
			});
		Parser parser(scanner);
		EXPECT_THROW({
			if (auto identifier = parser.tryBuildIdentifier(); identifier) {
				FAIL() << "Identifier building should fail";
			}
			}, SyntaxError);
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "roof"),
			Token(Token::TokenType::DOT, "."),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "house"),
			Token(Token::TokenType::DOT, ".")
			});
		Parser parser(scanner);
		EXPECT_THROW({
			if (auto identifier = parser.tryBuildIdentifier(); identifier) {
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

		if (auto value = parser.tryBuildValue(); value) {
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

		if (auto value = parser.tryBuildValue(); value) {
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

		if (auto value = parser.tryBuildValue(); value) {
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

		if (auto value = parser.tryBuildValue(); value) {
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
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "width")
		});
	Parser parser(scanner);

	Expression expr = DecimalValue(Position(), "12.4");
	if (auto addition = parser.tryBuildAddition(expr); addition) {
		EXPECT_TRUE(dynamic_cast<Sum*>(addition.get()) != nullptr);

		EXPECT_TRUE(std::holds_alternative<Identifier>(addition->getSecondExpression()));

		EXPECT_EQ(std::get<Identifier>(addition->getSecondExpression()).getValue(), "house");

		EXPECT_TRUE(std::holds_alternative<DecimalValue>(addition->getFirstExpression()));

		EXPECT_EQ(std::get<DecimalValue>(addition->getFirstExpression()).getValue(), "12.4");
	}
	else {
		FAIL() << "Expected addition";
	}
}

TEST(ParserUnitTests, CreateSubstraction) {
	ScannerMock scanner({
		Token(Token::TokenType::MINUS, "-"),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "width")
		});
	Parser parser(scanner);
	Expression expr = DecimalValue(Position(), "12.4");
	if (auto addition = parser.tryBuildAddition(expr); addition) {
		EXPECT_TRUE(dynamic_cast<Substraction*>(addition.get()) != nullptr);
	}
	else {
		FAIL() << "Expected substraction";
	}
}

TEST(ParserUnitTests, CreateMultiplication) {
	ScannerMock scanner({
		Token(Token::TokenType::ASTERISK, "*"),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "house")
		});
	Parser parser(scanner);

	Expression expr = DecimalValue(Position(), "12.4");
	if (auto multiplication = parser.tryBuildMultiplication(expr); multiplication) {
		EXPECT_TRUE(dynamic_cast<Multiplication_*>(multiplication.get()) != nullptr);

		EXPECT_TRUE(std::holds_alternative<Identifier>(multiplication->getSecondExpression()));

		EXPECT_EQ(std::get<Identifier>(multiplication->getSecondExpression()).getValue(), "house");
		EXPECT_TRUE(std::holds_alternative<DecimalValue>(multiplication->getFirstExpression()));
		EXPECT_EQ(std::get<DecimalValue>(multiplication->getFirstExpression()).getValue(), "12.4");
	}
	else {
		FAIL() << "Expected multiplication";
	}
}

TEST(ParserUnitTests, CreateDivision) {
	ScannerMock scanner({
		Token(Token::TokenType::SLASH, "//"),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "width")
		});
	Parser parser(scanner);
	Expression expr = DecimalValue(Position(), "12.4");
	if (auto multiplication = parser.tryBuildMultiplication(expr); multiplication) {
		EXPECT_TRUE(dynamic_cast<Division*>(multiplication.get()) != nullptr);
	}
	else {
		FAIL() << "Expected division";
	}
}


TEST(ParserUnitTests, CreateLogicalExpression) {
	ScannerMock scanner({
		Token(Token::TokenType::GREATER_OR_EQUAL, ">="),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "width"),
		Token(Token::TokenType::OR, "||"),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "height"),
		Token(Token::TokenType::LESS_THAN, "<"),
		Token(Token::TokenType::MINUS, "-"),
		Token(Token::TokenType::DECIMAL_CONST, "10.7"),
		Token(Token::TokenType::AND, "&&"),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "roof"),
		Token(Token::TokenType::EQUAL_SIGN, "="),
		Token(Token::TokenType::DECIMAL_CONST, "0.3")
		});
	Parser parser(scanner);
	Expression expr = DecimalValue(Position(), "12.4");
	/* expected structure:
		Logical OR : DisjunctionExpression
			-Comparison greater or equal:
				-DecimalValue 12.4
				-identifier width
			-Logical AND: ConjuctionExpression
				-Comparison less than:
					-identifier height
					-decimal const -10.7
				-Comparison Equal:
					-identifier roof
					-decimal const 0.3
	*/
	if (auto comparison = parser.tryBuildComparison(expr); comparison) {
		EXPECT_TRUE(dynamic_cast<GreaterOrEqual*>(comparison.get()));
		std::unique_ptr<LogicalSubExpression> sub = std::make_unique<LogicalSubExpression>(std::move(comparison));
	
		if (auto logicalExpr = parser.tryBuildLogicalExpression(sub); logicalExpr) { 
			EXPECT_TRUE(dynamic_cast<DisjunctionExpression*>(logicalExpr.get()));
			{//check left side - Comparison greater or equal
				auto& firstSubExpression = *logicalExpr->getFirstValue();

				EXPECT_TRUE(std::holds_alternative<ComparisonPtr>(firstSubExpression));
				auto& comp = std::get<ComparisonPtr>(firstSubExpression);

				EXPECT_EQ(std::get<DecimalValue>(comp->getFirstExpression()).getValue(), "12.4");
				EXPECT_EQ(std::get<Identifier>(comp->getSecondExpression()).getValue(), "width");
				EXPECT_TRUE(dynamic_cast<GreaterOrEqual*>(comp.get()));
			}
			{
				//check right side - Logical AND:
				LogicalSubExpression& secondSubExpression = *logicalExpr->getSecondValue();

				EXPECT_TRUE(std::holds_alternative<LogicalExpressionPtr>(secondSubExpression));
				auto& expr = std::get<LogicalExpressionPtr>(secondSubExpression);
				ConjuctionExpression* conj = dynamic_cast<ConjuctionExpression*>(expr.get());
				EXPECT_TRUE(conj);
				{//now check the left side of it - Comparison less than
					auto& firstSubExpression = *conj->getFirstValue();

					EXPECT_TRUE(std::holds_alternative<ComparisonPtr>(firstSubExpression));
					auto& comp = std::get<ComparisonPtr>(firstSubExpression);

					EXPECT_EQ(std::get<Identifier>(comp->getFirstExpression()).getValue(), "height");
					EXPECT_EQ(std::get<DecimalValue>(comp->getSecondExpression()).getValue(), "-10.7");
					EXPECT_TRUE(dynamic_cast<LessThan*>(comp.get()));
				}

				{//now check the right side - Comparison Equal
					auto& secondSubExpression = *conj->getSecondValue();

					EXPECT_TRUE(std::holds_alternative<ComparisonPtr>(secondSubExpression));
					auto& comp = std::get<ComparisonPtr>(secondSubExpression);

					EXPECT_EQ(std::get<Identifier>(comp->getFirstExpression()).getValue(), "roof");
					EXPECT_EQ(std::get<DecimalValue>(comp->getSecondExpression()).getValue(), "0.3");
					EXPECT_TRUE(dynamic_cast<Equal*>(comp.get()));
				}
			}
		}
		else {
			FAIL() << "Expected DisjunctionExpression";
		}
	}
	else
		FAIL() << "Expected equality comparison";
}


TEST(ParserUnitTests, CreateTernaryExpression) {
	ScannerMock scanner({
		Token(Token::TokenType::DECIMAL_CONST, "0.25"),
		Token(Token::TokenType::GREATER_OR_EQUAL, ">="),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "width"),
		Token(Token::TokenType::OR, "||"),
		Token(Token::TokenType::OPENING_BRACKET, "("),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "height"),
		Token(Token::TokenType::LESS_THAN, "<"),
		Token(Token::TokenType::MINUS, "-"),
		Token(Token::TokenType::DECIMAL_CONST, "10.7"),
		Token(Token::TokenType::AND, "&&"),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "roof"),
		Token(Token::TokenType::EQUAL_SIGN, "="),
		Token(Token::TokenType::DECIMAL_CONST, "0.3"),
		Token(Token::TokenType::CLOSING_BRACKET, ")"),
		Token(Token::TokenType::QUESTION_MARK, "?"),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "that"),
		Token(Token::TokenType::COLON, ":"),
		Token(Token::TokenType::VARIABLE_IDENTIFIER, "other")
		});
	Parser parser(scanner);
	Expression expr = DecimalValue(Position(), "12.4");
	/* expected structure:
	* TernaryExpression:
		Condition:
			Logical OR : DisjunctionExpression
				-Comparison greater or equal:
					-DecimalValue 0.25
					-identifier width
				-Logical AND: ConjuctionExpression
					-Comparison less than:
						-identifier height
						-decimal const -10.7
					-Comparison Equal:
						-identifier roof
						-decimal const 0.3
		ifTrue:
			identifier that
		ifFalse:
			ifentifier other
	*/
	if (auto ternary = parser.tryBuildExpression(); ternary) {
		auto& expr = std::get<TernaryExpressionPtr>(*ternary);
		{
			auto& condition = std::get<LogicalExpressionPtr>(*expr->getCondition());

			EXPECT_TRUE(dynamic_cast<DisjunctionExpression*>(condition.get()) != nullptr);
			{ //check conditions
				auto firstSubExpr = condition->getFirstValue();
				auto& comparison = std::get<ComparisonPtr>(*firstSubExpr);
				EXPECT_TRUE(dynamic_cast<GreaterOrEqual*>(comparison.get()));

				auto& comparExpr1 = comparison->getFirstExpression();

				auto& decVal = std::get<DecimalValue>(comparExpr1);
				EXPECT_EQ(decVal.getValue(), "0.25");

				auto& comparExpr2 = comparison->getSecondExpression();
				auto& identVal = std::get<Identifier>(comparExpr2);
				EXPECT_EQ(identVal.getValue(), "width");
			}
			{
				auto secondSubExpr = condition->getSecondValue();
				auto& conjExpr = std::get<LogicalExpressionPtr>(*secondSubExpr);
				ConjuctionExpression* conj = dynamic_cast<ConjuctionExpression*>(conjExpr.get());
				EXPECT_TRUE(conj);
				{//now check the left side of it - Comparison less than
					auto& firstSubExpression = *conj->getFirstValue();

					EXPECT_TRUE(std::holds_alternative<ComparisonPtr>(firstSubExpression));
					auto& comp = std::get<ComparisonPtr>(firstSubExpression);

					EXPECT_EQ(std::get<Identifier>(comp->getFirstExpression()).getValue(), "height");
					EXPECT_EQ(std::get<DecimalValue>(comp->getSecondExpression()).getValue(), "-10.7");
					EXPECT_TRUE(dynamic_cast<LessThan*>(comp.get()));
				}

				{//now check the right side - Comparison Equal
					auto& secondSubExpression = *conj->getSecondValue();

					EXPECT_TRUE(std::holds_alternative<ComparisonPtr>(secondSubExpression));
					auto& comp = std::get<ComparisonPtr>(secondSubExpression);

					EXPECT_EQ(std::get<Identifier>(comp->getFirstExpression()).getValue(), "roof");
					EXPECT_EQ(std::get<DecimalValue>(comp->getSecondExpression()).getValue(), "0.3");
					EXPECT_TRUE(dynamic_cast<Equal*>(comp.get()));
				}
			}
		}

		auto& ifTrue = expr->getTrueExpression();
		{
			auto& comp = std::get<Identifier>(ifTrue);
			EXPECT_EQ(comp.getValue(), "that");

		}


		auto& ifFalse = expr->getFalseExpression();
		{
			auto& comp = std::get<Identifier>(ifFalse);
			EXPECT_EQ(comp.getValue(), "other");
		}

	}	
}


TEST(ParserUnitTests, CreateComparison) {
	{
		ScannerMock scanner({
			Token(Token::TokenType::EQUAL_SIGN, "="),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "width")
			});
		Parser parser(scanner);
		Expression expr = DecimalValue(Position(), "12.4");
		if (auto comparison = parser.tryBuildComparison(expr); comparison) {
			EXPECT_TRUE(dynamic_cast<Equal*>(comparison.get()));
		}
		else
			FAIL() << "Expected equality comparison";
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::NOT_EQUAL, "!="),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "width")
			});
		Parser parser(scanner);
		Expression expr = DecimalValue(Position(), "12.4");
		if (auto comparison = parser.tryBuildComparison(expr); comparison) {
			EXPECT_TRUE(dynamic_cast<NotEqual*>(comparison.get()));
		}
		else
			FAIL() << "Expected not equality comparison";
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::LESS_OR_EQUAL, "<="),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "width")
			});
		Parser parser(scanner);
		Expression expr = DecimalValue(Position(), "12.4");
		if (auto comparison = parser.tryBuildComparison(expr); comparison) {
			EXPECT_TRUE(dynamic_cast<LessOrEqual*>(comparison.get()));
		}
		else
			FAIL() << "Expected less or equal comparison";
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::GREATER_OR_EQUAL, ">="),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "width")
			});
		Parser parser(scanner);
		Expression expr = DecimalValue(Position(), "12.4");
		if (auto comparison = parser.tryBuildComparison(expr); comparison) {
			EXPECT_TRUE(dynamic_cast<GreaterOrEqual*>(comparison.get()));
		}
		else
			FAIL() << "Expected greater or equal comparison";
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::LESS_THAN, "<"),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "width")
			});
		Parser parser(scanner);
		Expression expr = DecimalValue(Position(), "12.4");
		if (auto comparison = parser.tryBuildComparison(expr); comparison) {
			EXPECT_TRUE(dynamic_cast<LessThan*>(comparison.get()));
		}
		else
			FAIL() << "Expected equality comparison";
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::GREATER_THAN, ">"),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "width")
			});
		Parser parser(scanner);
		Expression expr = DecimalValue(Position(), "12.4");
		if (auto comparison = parser.tryBuildComparison(expr); comparison) {
			EXPECT_TRUE(dynamic_cast<GreaterThan*>(comparison.get()));
		}
		else
			FAIL() << "Expected equality comparison";
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::EQUAL_SIGN, "="),
			Token(Token::TokenType::EQUAL_SIGN, "+")
			});
		Parser parser(scanner);
		EXPECT_THROW({
		Expression expr = DecimalValue(Position(), "12.4");
			if (auto comparison = parser.tryBuildComparison(expr); comparison) {
				FAIL() << "Comparison building should fail";
			}
			}, SyntaxError);
	}
	{
		ScannerMock scanner({
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "fcdsad")
			});
		Parser parser(scanner);
		Expression expr = DecimalValue(Position(), "12.4");
		if (auto comparison = parser.tryBuildComparison(expr); comparison) {
			FAIL() << "Comparison building should return nullopt";
		}
	}
}


TEST(ParserUnitTests, CreateBasicObject) {
	{
		ScannerMock scanner({
			Token(Token::TokenType::TYPE_IDENTIFIER, "Circle"),
			Token(Token::TokenType::OPENING_BRACE, "{"),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "width"),
			Token(Token::TokenType::COLON, ":"),
			Token(Token::TokenType::DECIMAL_CONST, "12.7"),
			Token(Token::TokenType::ASTERISK, "*"),
			Token(Token::TokenType::MINUS, "-"),
			Token(Token::TokenType::DECIMAL_CONST, "10"),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "height"),
			Token(Token::TokenType::COLON, ":"),
			Token(Token::TokenType::VARIABLE_IDENTIFIER, "width"),
			Token(Token::TokenType::CLOSING_BRACE, "}")
			});
		Parser parser(scanner);
		if (auto obj = parser.tryBuildBasicObject(); obj) {
			EXPECT_NE(dynamic_cast<Circle*>(obj.get()), nullptr);
			auto& properties = obj->getProperties();
			EXPECT_EQ(properties.size(), 2);
			auto& expr1 = properties[0]->getValue();
			EXPECT_TRUE(std::holds_alternative<MultiplicationPtr>(expr1));
			auto& expr2 = properties[1]->getValue();
			EXPECT_TRUE(std::holds_alternative<Identifier>(expr2));
			EXPECT_EQ(std::get<Identifier>(expr2).getValue(), "width");
		}
		else {
			FAIL() << "Circle should be built";
		}
	}
	{
		ScannerMock scanner({
		Token(Token::TokenType::TYPE_IDENTIFIER, "Rectangle"),
		Token(Token::TokenType::OPENING_BRACE, "{"),
		Token(Token::TokenType::CLOSING_BRACE, "}")
			});
		Parser parser(scanner);
		if (auto obj = parser.tryBuildBasicObject(); obj) {
			EXPECT_NE(dynamic_cast<Rectangle*>(obj.get()), nullptr);
			auto& properties = obj->getProperties();
			EXPECT_EQ(properties.size(), 0);
		}
		else {
			FAIL() << "Rectangle should be built";
		}
	}
	{
		ScannerMock scanner({
		Token(Token::TokenType::TYPE_IDENTIFIER, "Line"),
		Token(Token::TokenType::OPENING_BRACE, "{"),
		Token(Token::TokenType::CLOSING_BRACE, "}")
			});
		Parser parser(scanner);
		if (auto obj = parser.tryBuildBasicObject(); obj) {
			EXPECT_NE(dynamic_cast<Line*>(obj.get()), nullptr);
			auto& properties = obj->getProperties();
			EXPECT_EQ(properties.size(), 0);
		}
		else {
			FAIL() << "Line should be built";
		}
	}
	{
		ScannerMock scanner({
		Token(Token::TokenType::TYPE_IDENTIFIER, "Polygon"),
		Token(Token::TokenType::OPENING_BRACE, "{"),
		Token(Token::TokenType::CLOSING_BRACE, "}")
			});
		Parser parser(scanner);
		if (auto obj = parser.tryBuildBasicObject(); obj) {
			EXPECT_NE(dynamic_cast<Polygon*>(obj.get()), nullptr);
			auto& properties = obj->getProperties();
			EXPECT_EQ(properties.size(), 0);
		}
		else {
			FAIL() << "Polygon should be built";
		}
	}
	{
		ScannerMock scanner({
		Token(Token::TokenType::TYPE_IDENTIFIER, "MyObject"),
		Token(Token::TokenType::OPENING_BRACE, "{"),
		Token(Token::TokenType::CLOSING_BRACE, "}")
			});
		Parser parser(scanner);
		if (auto obj = parser.tryBuildBasicObject(); obj) {
			FAIL() << "Should fail";
		}
	}
}

