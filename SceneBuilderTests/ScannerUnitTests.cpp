#include "pch.h"
#include <sstream>
#include "../SceneBuilder/Exceptions/TokenTooLong.h"
#include "../SceneBuilder/Scanner/Scanner.cpp"

TEST(ScannerUnitTest, EmptySourceTest) {
	std::stringstream stream("");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::END_OF_FILE);
	scanner.next();
	EXPECT_EQ(scanner.getToken().getType(), TokenType::END_OF_FILE);
}

TEST(ScannerUnitTest, CreateOpenBracketToken) {
	std::stringstream stream("(");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::OPENING_BRACKET);
}

TEST(ScannerUnitTest, CreateClosingBracketToken) {
	std::stringstream stream(")");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::CLOSING_BRACKET);
}

TEST(ScannerUnitTest, CreateOpeningBraceToken) {
	std::stringstream stream("{");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::OPENING_BRACE);
}

TEST(ScannerUnitTest, CreateClosingBraceToken) {
	std::stringstream stream("}");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::CLOSING_BRACE);
}

TEST(ScannerUnitTest, CreateColonToken) {
	std::stringstream stream(":");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::COLON);
}

TEST(ScannerUnitTest, CreateDotToken) {
	std::stringstream stream(".");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::DOT);
}

TEST(ScannerUnitTest, CreateCommaToken) {
	std::stringstream stream(",");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::COMMA);
}

TEST(ScannerUnitTest, CreateOpeningSquareBraceToken) {
	std::stringstream stream("[");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::OPENING_SQUARE_BRACE);
}

TEST(ScannerUnitTest, CreateClosingSquareBraceToken) {
	std::stringstream stream("]");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::CLOSING_SQUARE_BRACE);
}

TEST(ScannerUnitTest, CreatePlusToken) {
	std::stringstream stream("+");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::PLUS);
}

TEST(ScannerUnitTest, CreateMinusToken) {
	std::stringstream stream("-");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::MINUS);
}

TEST(ScannerUnitTest, CreateSlashToken) {
	std::stringstream stream("/");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::SLASH);
}

TEST(ScannerUnitTest, CreateAsteriskToken) {
	std::stringstream stream("*");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::ASTERISK);
}

TEST(ScannerUnitTest, CreateEqualSignToken) {
	std::stringstream stream("=");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::EQUAL_SIGN);
}

TEST(ScannerUnitTest, CreateQuestionMarkToken) {
	std::stringstream stream("?");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::QUESTION_MARK);
}

TEST(ScannerUnitTest, CreateLessThanToken) {
	std::stringstream stream("<");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::LESS_THAN);
}

TEST(ScannerUnitTest, CreateGreaterThanToken) {
	std::stringstream stream(">");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::GREATER_THAN);
}

TEST(ScannerUnitTest, CreateLessOrEqualToken) {
	std::stringstream stream("<=");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::LESS_OR_EQUAL);
}

TEST(ScannerUnitTest, CreateGreaterOrEqualToken) {
	std::stringstream stream(">=");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::GREATER_OR_EQUAL);
}

TEST(ScannerUnitTest, CreateNotEqualToken) {
	std::stringstream stream("!=");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::NOT_EQUAL);
}


TEST(ScannerUnitTest, CreateOrToken) {
	{
		std::stringstream stream("||");
		SceneBuilderScanner scanner(stream);
		EXPECT_EQ(scanner.getToken().getType(), TokenType::OR);
	}
	 try {
		std::stringstream stream("|sada");
		SceneBuilderScanner scanner(stream);
		EXPECT_EQ(scanner.getToken().getType(), TokenType::UNDEFINED);
		FAIL() << "Expected SyntaxError";
	 }
	 catch (const SyntaxError& err) {
		 EXPECT_EQ(err.expected, "|");
		 EXPECT_EQ(err.got, "s");
	 }
	 catch (...) {
		 FAIL() << "Expected SyntaxError";
	 }
}

TEST(ScannerUnitTest, CreateAndToken) {
	{
		std::stringstream stream("&&");
		SceneBuilderScanner scanner(stream);
		EXPECT_EQ(scanner.getToken().getType(), TokenType::AND);
	}
	try {
		std::stringstream stream("&sada");
		SceneBuilderScanner scanner(stream);
		FAIL() << "Expected SyntaxError";
	}
	catch (const SyntaxError& err) {
		EXPECT_EQ(err.expected, "&");
		EXPECT_EQ(err.got, "s");
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}

TEST(ScannerUnitTest, CreateHexConstToken) {
	{
		std::stringstream stream("#ABCDEF0123456789zzasd");
		SceneBuilderScanner scanner(stream);
		EXPECT_EQ(scanner.getToken().getType(), TokenType::HEX_CONST);
		EXPECT_EQ(scanner.getToken().getValue(), "0xABCDEF0123456789");
		scanner.next();
		EXPECT_NE(scanner.getToken().getType(), TokenType::HEX_CONST);
	}

	{
		std::stringstream stream("fw#ABCDEF0123456789 zzasd");
		SceneBuilderScanner scanner(stream);
		scanner.next();
		EXPECT_EQ(scanner.getToken().getType(), TokenType::HEX_CONST);
		EXPECT_EQ(scanner.getToken().getValue(), "0xABCDEF0123456789");
		scanner.next();
		EXPECT_NE(scanner.getToken().getType(), TokenType::HEX_CONST);
	}
}

TEST(ScannerUnitTest, CreateDecimalConstToken) {
	{
		std::stringstream stream("1 1. 1234567890.0987654321");
		SceneBuilderScanner scanner(stream);
		EXPECT_EQ(scanner.getToken().getType(), TokenType::DECIMAL_CONST);
		EXPECT_EQ(scanner.getToken().getValue(), "1");
		scanner.next();
		EXPECT_EQ(scanner.getToken().getType(), TokenType::DECIMAL_CONST);
		EXPECT_EQ(scanner.getToken().getValue(), "1.");
		scanner.next();
		EXPECT_EQ(scanner.getToken().getType(), TokenType::DECIMAL_CONST);
		EXPECT_EQ(scanner.getToken().getValue(), "1234567890.0987654321");
	}
}

TEST(ScannerUnitTest, CreateTypeIdentifierToken) {
	std::stringstream stream("Aqwertyuiopasdfghjklzxcvbnm1234567890_.");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::TYPE_IDENTIFIER);
	EXPECT_EQ(scanner.getToken().getValue(), "Aqwertyuiopasdfghjklzxcvbnm1234567890_");
}

TEST(ScannerUnitTest, CreateVariableIdentifierToken) {
	std::stringstream stream("aqwertyuiopasdfghjklzxcvbnm1234567890_.");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), TokenType::VARIABLE_IDENTIFIER);
	EXPECT_EQ(scanner.getToken().getValue(), "aqwertyuiopasdfghjklzxcvbnm1234567890_");
}

TEST(ScannerUnitTest, IncorrectHexTokenThrow) {
	std::stringstream stream("#zzasd");
	try {
		SceneBuilderScanner scanner(stream);
		FAIL() << "Expected SyntaxError";
	}
	catch (const SyntaxError& err) {
		EXPECT_EQ(err.got, "z");
		EXPECT_EQ(err.expected, "hexadecimal const value");
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}

TEST(ScannerUnitTest, TooLongHexValueThrow) {
	std::string buffer = "#" + std::string(SceneBuilderScanner::MAX_HEX_VALUE_LENGTH, 'A');
	EXPECT_EQ(buffer.size(), SceneBuilderScanner::MAX_HEX_VALUE_LENGTH + 1);
	std::stringstream stream(buffer.c_str());
	try {
		SceneBuilderScanner scanner(stream);
		FAIL() << "Expected TokenTooLong";
	}
	catch (const TokenTooLong& err) {
		EXPECT_EQ(err.got, "Hexadecimal value");
		EXPECT_EQ(buffer.size(), SceneBuilderScanner::MAX_HEX_VALUE_LENGTH + 1);
	}
	catch (...) {
		FAIL() << "Expected TokenTooLong";
	}
}

TEST(ScannerUnitTest, TooLongDecimalValueThrow) {
	std::string buffer(SceneBuilderScanner::MAX_DECIMAL_VALUE_LENGTH + 1, '1');
	EXPECT_EQ(buffer.size(), SceneBuilderScanner::MAX_DECIMAL_VALUE_LENGTH + 1);
	std::stringstream stream(buffer.c_str());
	try {
		SceneBuilderScanner scanner(stream);
		FAIL() << "Expected TokenTooLong";
	}
	catch (const TokenTooLong& err) {
		EXPECT_EQ(err.got, "Decimal value");
	}
	catch (...) {
		FAIL() << "Expected TokenTooLong";
	}

	//same but with a dot
	buffer[SceneBuilderScanner::MAX_DECIMAL_VALUE_LENGTH / 2] = '.';
	std::stringstream stream2(buffer.c_str());
	try {
		SceneBuilderScanner scanner(stream2);
		FAIL() << "Expected TokenTooLong";
	}
	catch (const TokenTooLong& err) {
		EXPECT_EQ(err.got, "Decimal value");
		EXPECT_EQ(buffer.size(), SceneBuilderScanner::MAX_DECIMAL_VALUE_LENGTH + 1);
	}
	catch (...) {
		FAIL() << "Expected TokenTooLong";
	}
}

TEST(ScannerUnitTest, TooLongVariableNameThrow) {
	std::string buffer(SceneBuilderScanner::MAX_NAME_LENGTH+1, 'a');
	EXPECT_EQ(buffer.size(), SceneBuilderScanner::MAX_NAME_LENGTH + 1);
	std::stringstream stream(buffer.c_str());
	try {
		SceneBuilderScanner scanner(stream);
		FAIL() << "Expected TokenTooLong";
	}
	catch (const TokenTooLong& err) {
		EXPECT_EQ(err.got, "Variable name");
	}
	catch (...) {
		FAIL() << "Expected TokenTooLong";
	}
}

TEST(ScannerUnitTest, TooLongTypeNameThrow) {
	std::string buffer = "A" + std::string(SceneBuilderScanner::MAX_NAME_LENGTH, 'a');
	EXPECT_EQ(buffer.size(), SceneBuilderScanner::MAX_NAME_LENGTH + 1);
	std::stringstream stream(buffer.c_str());
	try {
		SceneBuilderScanner scanner(stream);
		FAIL() << "Expected TokenTooLong";
	}
	catch (const TokenTooLong& err) {
		EXPECT_EQ(err.got, "Type name");
	}
	catch (...) {
		FAIL() << "Expected TokenTooLong";
	}
}

TEST(ScannerUnitTest, TooLongSpaceThrow) {
	std::string buffer(SceneBuilderScanner::MAX_EMPTY_SPACE_LENGTH + 1, ' ');
	EXPECT_EQ(buffer.size(), SceneBuilderScanner::MAX_EMPTY_SPACE_LENGTH + 1);
	std::stringstream stream(buffer.c_str());
	try {
		SceneBuilderScanner scanner(stream);
		FAIL() << "Expected TokenTooLong";
	}
	catch (const TokenTooLong& err) {
		EXPECT_EQ(err.got, "Blank characters");
	}
	catch (...) {
		FAIL() << "Expected TokenTooLong";
	}
}