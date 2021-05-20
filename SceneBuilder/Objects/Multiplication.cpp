#include <array>
#include <algorithm>
#include "Multiplication.h"

bool MultiplicationFactory::isMultiplicationOperator(const TokenType& oper) {
	std::array<TokenType, 2> tokens = {
	TokenType::ASTERISK,
	TokenType::SLASH
	};
	auto res = std::find(tokens.begin(), tokens.end(), oper);
	return res != tokens.end();

}
std::unique_ptr<Multiplication> MultiplicationFactory::getMultiplication(const Position& position, Expression& expr1, Expression& expr2, const TokenType& oper) {
	switch (oper) {
	case TokenType::ASTERISK:
		return std::make_unique<Multiplication_>(position, expr1, expr2);
	case TokenType::SLASH:
		return std::make_unique<Division>(position, expr1, expr2);
	default:
		return nullptr;
	}
}


std::map<std::pair<ReturnType, ReturnType>, ReturnType>& Multiplication_::getMultiplicationResult() {
	return multiplicationResult;
}

std::map<std::pair<ReturnType, ReturnType>, ReturnType>& Division::getMultiplicationResult() {
	return divisionResult;
}

std::map<std::pair<ReturnType, ReturnType>, ReturnType> Multiplication_::multiplicationResult = {
	{{ReturnType::DECIMAL_VALUE,ReturnType::DECIMAL_VALUE}, ReturnType::DECIMAL_VALUE},
	{{ReturnType::DECIMAL_VALUE,ReturnType::POINT}, ReturnType::POINT},
	{{ReturnType::POINT,ReturnType::DECIMAL_VALUE}, ReturnType::POINT},
	{{ReturnType::COLOR,ReturnType::DECIMAL_VALUE}, ReturnType::COLOR},
	{{ReturnType::DECIMAL_VALUE,ReturnType::COLOR}, ReturnType::COLOR},
	{{ReturnType::TIME_DECLARATION, ReturnType::DECIMAL_VALUE}, ReturnType::TIME_DECLARATION},
	{{ReturnType::DECIMAL_VALUE, ReturnType::TIME_DECLARATION}, ReturnType::TIME_DECLARATION},
	{{ReturnType::POINT_ARRAY, ReturnType::DECIMAL_VALUE}, ReturnType::POINT_ARRAY},
	{{ReturnType::DECIMAL_VALUE, ReturnType::POINT_ARRAY}, ReturnType::POINT_ARRAY}
};

std::map<std::pair<ReturnType, ReturnType>, ReturnType> Division::divisionResult = {
	{{ReturnType::DECIMAL_VALUE,ReturnType::DECIMAL_VALUE}, ReturnType::DECIMAL_VALUE},
	{{ReturnType::POINT,ReturnType::DECIMAL_VALUE}, ReturnType::POINT},
	{{ReturnType::COLOR,ReturnType::DECIMAL_VALUE}, ReturnType::COLOR},
	{{ReturnType::TIME_DECLARATION, ReturnType::DECIMAL_VALUE}, ReturnType::TIME_DECLARATION},
	{{ReturnType::POINT_ARRAY, ReturnType::DECIMAL_VALUE}, ReturnType::POINT_ARRAY},
};

ReturnType Multiplication::getReturnType(ReturnType first, ReturnType second) {
	auto pair = std::make_pair(first, second);
	auto& multiplicationResult = getMultiplicationResult();
	auto res = std::find_if(multiplicationResult.begin(), multiplicationResult.end(), [=](const auto& obj) {
		return obj.first == pair;
	});
	if (res != multiplicationResult.end()) {
		return res->second;
	}
	return ReturnType::NON_TYPE;
}