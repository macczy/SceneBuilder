
#include "pch.h"
#include "../SceneBuilder/Scanner/Token.cpp"

TEST(TokenUnitTests, defaultConstructorTest) {
	Token token;
	EXPECT_EQ(token.getType(), Token::TokenType::UNDEFINED);
}
