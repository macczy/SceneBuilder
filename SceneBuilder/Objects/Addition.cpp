#include <array>
#include "Addition.h"

bool AdditionFactory::isAdditionOperator(const TokenType& oper) {
	return oper == TokenType::PLUS || oper == TokenType::MINUS;
}
AdditionPtr AdditionFactory::getAddition(const Position& position, Expression& expr1, Expression& expr2, const TokenType& oper) {
	switch (oper) {
	case TokenType::PLUS:
		return std::make_unique<Sum>(position, expr1, expr2);
	case TokenType::MINUS:
		return std::make_unique<Substraction>(position, expr1, expr2);
	default:
		return nullptr;
	}
}

std::map<std::pair<ReturnType, ReturnType>, ReturnType>& Sum::getAdditionResult() {
	return sumResult;
}

std::map<std::pair<ReturnType, ReturnType>, ReturnType>& Substraction::getAdditionResult() {
	return substractionResult;
}

std::map<std::pair<ReturnType, ReturnType>, ReturnType> Sum::sumResult = {
	{{ReturnType::DECIMAL_VALUE,ReturnType::DECIMAL_VALUE}, ReturnType::DECIMAL_VALUE},
	{{ReturnType::DECIMAL_VALUE,ReturnType::POINT}, ReturnType::POINT},
	{{ReturnType::POINT,ReturnType::DECIMAL_VALUE}, ReturnType::POINT},
	{{ReturnType::POINT,ReturnType::POINT}, ReturnType::POINT},
	{{ReturnType::COLOR,ReturnType::DECIMAL_VALUE}, ReturnType::COLOR},
	{{ReturnType::DECIMAL_VALUE,ReturnType::COLOR}, ReturnType::COLOR},
	{{ReturnType::COLOR,ReturnType::COLOR}, ReturnType::COLOR},
	{{ReturnType::TIME_DECLARATION, ReturnType::TIME_DECLARATION}, ReturnType::TIME_DECLARATION},
	{{ReturnType::POINT_ARRAY, ReturnType::DECIMAL_VALUE}, ReturnType::POINT_ARRAY},
	{{ReturnType::DECIMAL_VALUE, ReturnType::POINT_ARRAY}, ReturnType::POINT_ARRAY}
};

std::map<std::pair<ReturnType, ReturnType>, ReturnType> Substraction::substractionResult = {
	{{ReturnType::DECIMAL_VALUE,ReturnType::DECIMAL_VALUE}, ReturnType::DECIMAL_VALUE},
	{{ReturnType::POINT,ReturnType::DECIMAL_VALUE}, ReturnType::POINT},
	{{ReturnType::POINT,ReturnType::POINT}, ReturnType::POINT},
	{{ReturnType::COLOR,ReturnType::DECIMAL_VALUE}, ReturnType::COLOR},
	{{ReturnType::COLOR,ReturnType::COLOR}, ReturnType::COLOR},
	{{ReturnType::TIME_DECLARATION, ReturnType::TIME_DECLARATION}, ReturnType::TIME_DECLARATION},
	{{ReturnType::POINT_ARRAY, ReturnType::DECIMAL_VALUE}, ReturnType::POINT_ARRAY}
};

ReturnType Addition::getReturnType(ReturnType first, ReturnType second) {
	auto pair = std::make_pair(first, second);
	auto& additionResult = getAdditionResult();
	auto res = std::find_if(additionResult.begin(), additionResult.end(), [=](const auto& obj) {
		return obj.first == pair;
	});
	if (res != additionResult.end()) {
		return res->second;
	}
	return ReturnType::NON_TYPE;
}