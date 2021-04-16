#include "pch.h"
#include <sstream>
#include "../SceneBuilder/SyntaxError.h"
#include "../SceneBuilder/Scanner.cpp"

TEST(ScannerUnitTest, EmptySourceTest) {
	std::stringstream stream("");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::END_OF_FILE);
	scanner.next();
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::END_OF_FILE);
}

TEST(ScannerUnitTest, CreateOpenBracketToken) {
	std::stringstream stream("(");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::OPENING_BRACKET);
}

TEST(ScannerUnitTest, CreateClosingBracketToken) {
	std::stringstream stream(")");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::CLOSING_BRACKET);
}

TEST(ScannerUnitTest, CreateOpeningBraceToken) {
	std::stringstream stream("{");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::OPENING_BRACE);
}
TEST(ScannerUnitTest, CreateClosingBraceToken) {
	std::stringstream stream("}");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::CLOSING_BRACE);
}

TEST(ScannerUnitTest, CreateColonToken) {
	std::stringstream stream(":");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::COLON);
}

TEST(ScannerUnitTest, CreateDotToken) {
	std::stringstream stream(".");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::DOT);
}

TEST(ScannerUnitTest, CreateCommaToken) {
	std::stringstream stream(",");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::COMMA);
}

TEST(ScannerUnitTest, CreateOpeningSquareBraceToken) {
	std::stringstream stream("[");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::OPENING_SQUARE_BRACE);
}

TEST(ScannerUnitTest, CreateClosingSquareBraceToken) {
	std::stringstream stream("]");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::CLOSING_SQUARE_BRACE);
}

TEST(ScannerUnitTest, CreatePlusToken) {
	std::stringstream stream("+");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::PLUS);
}

TEST(ScannerUnitTest, CreateMinusToken) {
	std::stringstream stream("-");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::MINUS);
}

TEST(ScannerUnitTest, CreateSlashToken) {
	std::stringstream stream("/");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::SLASH);
}

TEST(ScannerUnitTest, CreateAsteriskToken) {
	std::stringstream stream("*");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::ASTERISK);
}

TEST(ScannerUnitTest, CreateEqualSignToken) {
	std::stringstream stream("=");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::EQUAL_SIGN);
}

TEST(ScannerUnitTest, CreateQuestionMarkToken) {
	std::stringstream stream("?");
	Scanner scanner(stream);
	EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::QUESTION_MARK);
}


TEST(ScannerUnitTest, CreateHexConstToken) {
	{
		std::stringstream stream("#ABCDEF0123456789zzasd");
		Scanner scanner(stream);
		EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::HEX_CONST);
		EXPECT_EQ(scanner.getToken().getValue(), "#ABCDEF0123456789");
		scanner.next();
		EXPECT_NE(scanner.getToken().getType(), Token::TokenType::HEX_CONST);
	}

	{
		std::stringstream stream("fw #ABCDEF0123456789 zzasd");
		Scanner scanner(stream);
		scanner.next();
		EXPECT_EQ(scanner.getToken().getType(), Token::TokenType::HEX_CONST);
		EXPECT_EQ(scanner.getToken().getValue(), "#ABCDEF0123456789");
		scanner.next();
		EXPECT_NE(scanner.getToken().getType(), Token::TokenType::HEX_CONST);
	}
}

TEST(ScannerUnitTest, IncorrectDecimalTokenThrow) {
	std::stringstream stream("1.dw");
	try {
		Scanner scanner(stream);
		FAIL() << "Expected runtime_error";
	}
	catch (SyntaxError const& err) {
		std::string error_message = "Expected number but got 'd' " + getPositionInSourceString(Token::Position{ 0, 2, 0 });
		EXPECT_EQ(err.what(), error_message);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}

TEST(ScannerUnitTest, IncorrectHexTokenThrow) {
	std::stringstream stream("#zzasd");
	try {
		Scanner scanner(stream);
		FAIL() << "Expected runtime_error";
	}
	catch (SyntaxError const& err) {
		std::string error_message = "Expected hexadecimal const value, but got 'z' " + getPositionInSourceString(Token::Position{ 0, 1, 0 });
		EXPECT_EQ(err.what(), error_message);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}

TEST(ScannerUnitTest, TooLongHexValueThrow) {
	std::string buffer = "#" + std::string(Scanner::MAX_HEX_VALUE_LENGTH, 'A');
	EXPECT_EQ(buffer.size(), Scanner::MAX_HEX_VALUE_LENGTH + 1);
	std::stringstream stream(buffer.c_str());
	try {
		Scanner scanner(stream);
		FAIL() << "Expected runtime_error";
	}
	catch (SyntaxError const& err) {
		std::string error_message = "Hexadecimal value exceeded maximum length " + getPositionInSourceString(Token::Position{ 0, 1, 0 });
		EXPECT_EQ(err.what(), error_message);
		EXPECT_EQ(buffer.size(), Scanner::MAX_HEX_VALUE_LENGTH + 1);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}


TEST(ScannerUnitTest, TooLongDecimalValueThrow) {
	std::string buffer(Scanner::MAX_DECIMAL_VALUE_LENGTH + 1, '1');
	EXPECT_EQ(buffer.size(), Scanner::MAX_DECIMAL_VALUE_LENGTH + 1);
	std::stringstream stream(buffer.c_str());
	try {
		Scanner scanner(stream);
		FAIL() << "Expected runtime_error";
	}
	catch (SyntaxError const& err) {
		std::string error_message = "Decimal value exceeded maximum length " + getPositionInSourceString(Token::Position{ 0, 1, 0 });
		EXPECT_EQ(err.what(), error_message);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}

	//same but with a dot
	buffer[Scanner::MAX_DECIMAL_VALUE_LENGTH / 2] = '.';
	std::stringstream stream2(buffer.c_str());
	try {
		Scanner scanner(stream2);
		FAIL() << "Expected runtime_error";
	}
	catch (SyntaxError const& err) {
		std::string error_message = "Decimal value exceeded maximum length " + getPositionInSourceString(Token::Position{ 0, 1, 0 });
		EXPECT_EQ(err.what(), error_message);
		EXPECT_EQ(buffer.size(), Scanner::MAX_DECIMAL_VALUE_LENGTH + 1);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}

TEST(ScannerUnitTest, TooLongVariableNameThrow) {
	std::string buffer(Scanner::MAX_NAME_LENGTH+1, 'a');
	EXPECT_EQ(buffer.size(), Scanner::MAX_NAME_LENGTH + 1);
	std::stringstream stream(buffer.c_str());
	try {
		Scanner scanner(stream);
		FAIL() << "Expected runtime_error";
	}
	catch (SyntaxError const& err) {
		std::string error_message = "Variable name exceeded maximum length " + getPositionInSourceString(Token::Position{ 0, 1, 0 });
		EXPECT_EQ(err.what(), error_message);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}


TEST(ScannerUnitTest, TooLongTypeNameThrow) {
	std::string buffer = "A" + std::string(Scanner::MAX_NAME_LENGTH, 'a');
	EXPECT_EQ(buffer.size(), Scanner::MAX_NAME_LENGTH + 1);
	std::stringstream stream(buffer.c_str());
	try {
		Scanner scanner(stream);
		FAIL() << "Expected runtime_error";
	}
	catch (SyntaxError const& err) {
		std::string error_message = "Type name exceeded maximum length " + getPositionInSourceString(Token::Position{ 0, 1, 0 });
		EXPECT_EQ(err.what(), error_message);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}


TEST(ScannerUnitTest, TooLongSpaceThrow) {
	std::string buffer(Scanner::MAX_EMPTY_SPACE_LENGTH + 1, ' ');
	EXPECT_EQ(buffer.size(), Scanner::MAX_EMPTY_SPACE_LENGTH + 1);
	std::stringstream stream(buffer.c_str());
	try {
		Scanner scanner(stream);
		FAIL() << "Expected runtime_error";
	}
	catch (SyntaxError const& err) {
		std::string error_message = "Expected token, got " + std::to_string(Scanner::MAX_EMPTY_SPACE_LENGTH + 1) + " blank characters " + getPositionInSourceString(Token::Position{ 0, 1, 0 });
		EXPECT_EQ(err.what(), error_message);
	}
	catch (...) {
		FAIL() << "Expected SyntaxError";
	}
}