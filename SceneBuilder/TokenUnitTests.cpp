#include <boost/test/unit_test.hpp>

#include "Token.h"


BOOST_AUTO_TEST_SUITE(TokenUnitTests)

BOOST_AUTO_TEST_CASE(defaultConstructorTest)
{
	Token token;
	BOOST_TEST(token.getType() == Token::TokenType::UNKNOWN);
	BOOST_TEST(token.getValue() == "");
}

BOOST_AUTO_TEST_CASE(someOtgerTokenTests)
{
	BOOST_TEST(false);
}

BOOST_AUTO_TEST_SUITE_END()