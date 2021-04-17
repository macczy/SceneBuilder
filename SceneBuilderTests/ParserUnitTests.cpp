#include "pch.h"
#include <deque>
#include "../SceneBuilder/Parser.cpp"

class ScannerMock : public Scanner {
public:
	ScannerMock(std::deque<Token> tokens, std::stringstream& str = std::stringstream("")) : tokens(tokens), Scanner(str) {
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
	ScannerMock scanner({ Token(Token::TokenType::UNDEFINED, "") });
	Parser parser(scanner);
	SceneRoot root = parser.parse();
}
