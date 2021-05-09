#include "Expression.h"

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;

const Position& getExpressionPosition(const Expression& val) {
	return std::visit( overload {
		[](const DecimalValue& value) { return value.getPosition(); },
		[](const HexValue& value) { return value.getPosition(); },
		[](const Color& value) { return value.getPosition(); },
		[](const Point& value) { return value.getPosition(); },
		[](const Identifier& value) { return value.getPosition(); },
		[](const TernaryExpressionPtr& value) { return value->getPosition(); },
		[](const MultiplicationPtr& value) { return value->getPosition(); },
		[](const AdditionPtr& value) { return value->getPosition(); }
		}, val);
}