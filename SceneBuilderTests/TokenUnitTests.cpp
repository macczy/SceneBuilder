
#include "pch.h"

TEST(TokenUnitTests, defaultConstructorTest) {
	Token token;
	EXPECT_EQ(token.getType(), TokenType::UNDEFINED);
}
