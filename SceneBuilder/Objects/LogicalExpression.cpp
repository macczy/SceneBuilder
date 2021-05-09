#include <array>
#include "LogicalExpression.h"

bool LogicalExpressionFactory::isLogicalOperator(const Token::TokenType& oper) {
	std::array<Token::TokenType, 2> tokens = {
		Token::TokenType::OR,
		Token::TokenType::AND,
	};
	auto res = std::find(tokens.begin(), tokens.end(), oper);
	return res != tokens.end();
}

const Position& getLogicalSubExpressionPosition(const LogicalSubExpression& val) {
	return std::visit([](auto&& value) { return value->getPosition(); }, val);
}