
#include "pch.h"
#include "../SceneBuilder/Util/Token.cpp"

TEST(TokenUnitTests, defaultConstructorTest) {
	Token token;
	EXPECT_EQ(token.getType(), TokenType::UNDEFINED);
}
