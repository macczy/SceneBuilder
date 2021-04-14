
#include "pch.h"
#include "../SceneBuilder/Token.cpp"

TEST(defaultConstructorTest, TokenUnitTests) {
	Token token;
	EXPECT_EQ(token.getType(), Token::TokenType::UNDEFINED);
}
