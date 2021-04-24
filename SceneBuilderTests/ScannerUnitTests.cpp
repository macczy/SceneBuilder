#include "pch.h"
#include <sstream>
#include "../SceneBuilder/SyntaxError.h"
#include "../SceneBuilder/Scanner/Scanner.cpp"

TEST(ScannerUnitTest, EmptySourceTest) {
	std::stringstream stream("");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::END_OF_FILE);
	scanner.next();
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::END_OF_FILE);
}

TEST(ScannerUnitTest, CreateOpenBracketToken) {
	std::stringstream stream("(");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::OPENING_BRACKET);
}

TEST(ScannerUnitTest, CreateClosingBracketToken) {
	std::stringstream stream(")");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::CLOSING_BRACKET);
}

TEST(ScannerUnitTest, CreateOpeningBraceToken) {
	std::stringstream stream("{");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::OPENING_BRACE);
}

TEST(ScannerUnitTest, CreateClosingBraceToken) {
	std::stringstream stream("}");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::CLOSING_BRACE);
}

TEST(ScannerUnitTest, CreateColonToken) {
	std::stringstream stream(":");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::COLON);
}

TEST(ScannerUnitTest, CreateDotToken) {
	std::stringstream stream(".");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::DOT);
}

TEST(ScannerUnitTest, CreateCommaToken) {
	std::stringstream stream(",");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::COMMA);
}

TEST(ScannerUnitTest, CreateOpeningSquareBraceToken) {
	std::stringstream stream("[");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::OPENING_SQUARE_BRACE);
}

TEST(ScannerUnitTest, CreateClosingSquareBraceToken) {
	std::stringstream stream("]");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::CLOSING_SQUARE_BRACE);
}

TEST(ScannerUnitTest, CreatePlusToken) {
	std::stringstream stream("+");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::PLUS);
}

TEST(ScannerUnitTest, CreateMinusToken) {
	std::stringstream stream("-");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::MINUS);
}

TEST(ScannerUnitTest, CreateSlashToken) {
	std::stringstream stream("/");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::SLASH);
}

TEST(ScannerUnitTest, CreateAsteriskToken) {
	std::stringstream stream("*");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::ASTERISK);
}

TEST(ScannerUnitTest, CreateEqualSignToken) {
	std::stringstream stream("=");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::EQUAL_SIGN);
}

TEST(ScannerUnitTest, CreateQuestionMarkToken) {
	std::stringstream stream("?");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::QUESTION_MARK);
}

TEST(ScannerUnitTest, CreateLessThanToken) {
	std::stringstream stream("<");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::LESS_THAN);
}

TEST(ScannerUnitTest, CreateGreaterThanToken) {
	std::stringstream stream(">");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::GREATER_THAN);
}

TEST(ScannerUnitTest, CreateLessOrEqualToken) {
	std::stringstream stream("<=");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::LESS_OR_EQUAL);
}

TEST(ScannerUnitTest, CreateGreaterOrEqualToken) {
	std::stringstream stream(">=");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::GREATER_OR_EQUAL);
}

TEST(ScannerUnitTest, CreateNotEqualToken) {
	std::stringstream stream("!=");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::NOT_EQUAL);
}


TEST(ScannerUnitTest, CreateOrToken) {
	{
		std::stringstream stream("||");
		SceneBuilderScanner scanner(stream);
		EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::OR);
	}
	 try {
		std::stringstream stream("|sada");
		SceneBuilderScanner scanner(stream);
		EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::UNDEFINED);
		FAIL() << "Expected SyntaxError";
	 }
	 catch (const SyntaxError& err) {
		 std::string error_message = "Expected | but got 's' " + Position{ 1, 2, 0 }.toString();
		 EXPECT_EQ(err.what(), error_message);
	 }
	 catch (...) {
		 FAIL() << "Expected SyntaxError";
	 }
}

TEST(ScannerUnitTest, CreateAndToken) {
	{
		std::stringstream stream("&&");
		SceneBuilderScanner scanner(stream);
		EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::AND);
	}
	try {
		std::stringstream stream("&sada");
		SceneBuilderScanner scanner(stream);
		FAIL() << "Expected SyntaxError";
	}
	catch (const SyntaxError& err) {
		std::string error_message = "Expected & but got 's' " + Position{ 1, 2, 0 }.toString();
		EXPECT_EQ(err.what(), error_message);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}

TEST(ScannerUnitTest, CreateHexConstToken) {
	{
		std::stringstream stream("#ABCDEF0123456789zzasd");
		SceneBuilderScanner scanner(stream);
		EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::HEX_CONST);
		EXPECT_EQ(scanner.getToken().getValue(), "#ABCDEF0123456789");
		scanner.next();
		EXPECT_NE(scanner.getToken().getType(), Token::TokenType::HEX_CONST);
	}

	{
		std::stringstream stream("fw#ABCDEF0123456789 zzasd");
		SceneBuilderScanner scanner(stream);
		scanner.next();
		EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::HEX_CONST);
		EXPECT_EQ(scanner.getToken().getValue(), "#ABCDEF0123456789");
		scanner.next();
		EXPECT_NE(scanner.getToken().getType(), Token::TokenType::HEX_CONST);
	}
}

TEST(ScannerUnitTest, CreateDecimalConstToken) {
	{
		std::stringstream stream("1 1. 1234567890.0987654321");
		SceneBuilderScanner scanner(stream);
		EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::DECIMAL_CONST);
		EXPECT_EQ(scanner.getToken().getValue(), "1");
		scanner.next();
		EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::DECIMAL_CONST);
		EXPECT_EQ(scanner.getToken().getValue(), "1.");
		scanner.next();
		EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::DECIMAL_CONST);
		EXPECT_EQ(scanner.getToken().getValue(), "1234567890.0987654321");
	}

	{
		std::stringstream stream("fw#ABCDEF0123456789 zzasd");
		SceneBuilderScanner scanner(stream);
		scanner.next();
		EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::HEX_CONST);
		EXPECT_EQ(scanner.getToken().getValue(), "#ABCDEF0123456789");
		scanner.next();
		EXPECT_NE(scanner.getToken().getType(), Token::TokenType::HEX_CONST);
	}
}

TEST(ScannerUnitTest, CreateTypeIdentifierToken) {
	std::stringstream stream("Aqwertyuiopasdfghjklzxcvbnm1234567890_.");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::TYPE_IDENTIFIER);
	EXPECT_EQ(scanner.getToken().getValue(), "Aqwertyuiopasdfghjklzxcvbnm1234567890_");
}

TEST(ScannerUnitTest, CreateVariableIdentifierToken) {
	std::stringstream stream("aqwertyuiopasdfghjklzxcvbnm1234567890_.");
	SceneBuilderScanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::VARIABLE_IDENTIFIER);
	EXPECT_EQ(scanner.getToken().getValue(), "aqwertyuiopasdfghjklzxcvbnm1234567890_");
}

TEST(ScannerUnitTest, IncorrectHexTokenThrow) {
	std::stringstream stream("#zzasd");
	try {
		SceneBuilderScanner scanner(stream);
		FAIL() << "Expected SyntaxError";
	}
	catch (const SyntaxError& err) {
		std::string error_message = "Expected hexadecimal const value, but got 'z' " + Position{ 1, 2, 0 }.toString() + "\n#zzasd\n ^";
		EXPECT_EQ(err.what(), error_message);
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
		FAIL() << "Expected runtime_error";
	}
	catch (const SyntaxError& err) {
		std::string error_message = "Hexadecimal value exceeded maximum length " + Position{ 1, 1, 0 }.toString();
		EXPECT_EQ(err.what(), error_message);
		EXPECT_EQ(buffer.size(), SceneBuilderScanner::MAX_HEX_VALUE_LENGTH + 1);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}

TEST(ScannerUnitTest, TooLongDecimalValueThrow) {
	std::string buffer(SceneBuilderScanner::MAX_DECIMAL_VALUE_LENGTH + 1, '1');
	EXPECT_EQ(buffer.size(), SceneBuilderScanner::MAX_DECIMAL_VALUE_LENGTH + 1);
	std::stringstream stream(buffer.c_str());
	try {
		SceneBuilderScanner scanner(stream);
		FAIL() << "Expected runtime_error";
	}
	catch (const SyntaxError& err) {
		std::string error_message = "Decimal value exceeded maximum length " + Position{ 1, 1, 0 }.toString();
		EXPECT_EQ(err.what(), error_message);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}

	//same but with a dot
	buffer[SceneBuilderScanner::MAX_DECIMAL_VALUE_LENGTH / 2] = '.';
	std::stringstream stream2(buffer.c_str());
	try {
		SceneBuilderScanner scanner(stream2);
		FAIL() << "Expected runtime_error";
	}
	catch (const SyntaxError& err) {
		std::string error_message = "Decimal value exceeded maximum length " + Position{ 1, 1, 0 }.toString();
		EXPECT_EQ(err.what(), error_message);
		EXPECT_EQ(buffer.size(), SceneBuilderScanner::MAX_DECIMAL_VALUE_LENGTH + 1);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}

TEST(ScannerUnitTest, TooLongVariableNameThrow) {
	std::string buffer(SceneBuilderScanner::MAX_NAME_LENGTH+1, 'a');
	EXPECT_EQ(buffer.size(), SceneBuilderScanner::MAX_NAME_LENGTH + 1);
	std::stringstream stream(buffer.c_str());
	try {
		SceneBuilderScanner scanner(stream);
		FAIL() << "Expected runtime_error";
	}
	catch (const SyntaxError& err) {
		std::string error_message = "Variable name exceeded maximum length " + Position{ 1, 1, 0 }.toString();
		EXPECT_EQ(err.what(), error_message);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}

TEST(ScannerUnitTest, TooLongTypeNameThrow) {
	std::string buffer = "A" + std::string(SceneBuilderScanner::MAX_NAME_LENGTH, 'a');
	EXPECT_EQ(buffer.size(), SceneBuilderScanner::MAX_NAME_LENGTH + 1);
	std::stringstream stream(buffer.c_str());
	try {
		SceneBuilderScanner scanner(stream);
		FAIL() << "Expected runtime_error";
	}
	catch (const SyntaxError& err) {
		std::string error_message = "Type name exceeded maximum length " + Position{ 1, 1, 0 }.toString();
		EXPECT_EQ(err.what(), error_message);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}

TEST(ScannerUnitTest, TooLongSpaceThrow) {
	std::string buffer(SceneBuilderScanner::MAX_EMPTY_SPACE_LENGTH + 1, ' ');
	EXPECT_EQ(buffer.size(), SceneBuilderScanner::MAX_EMPTY_SPACE_LENGTH + 1);
	std::stringstream stream(buffer.c_str());
	try {
		SceneBuilderScanner scanner(stream);
		FAIL() << "Expected runtime_error";
	}
	catch (const SyntaxError& err) {
		std::string error_message = "Expected token, got " + std::to_string(SceneBuilderScanner::MAX_EMPTY_SPACE_LENGTH + 1) + " blank characters " + Position{ 1, 1, 0 }.toString();
		EXPECT_EQ(err.what(), error_message);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}