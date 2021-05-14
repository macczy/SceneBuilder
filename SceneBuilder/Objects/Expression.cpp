#include "Expression.h"

struct ExpressionPositionVisitor {
	const Position& operator()(const DecimalValue& value) { return value.getPosition(); }
	const Position& operator()(const HexValue& value) { return value.getPosition(); }
	const Position& operator()(const Color& value) { return value.getPosition(); }
	const Position& operator()(const Point& value) { return value.getPosition(); }
	const Position& operator()(const PointArray& value) { return value.getPosition(); }
	const Position& operator()(const Identifier& value) { return value.getPosition(); }
	const Position& operator()(const ConstantIdentifier& value) { return value.getPosition(); }
	const Position& operator()(const TimeDeclaration& value) { return value.getPosition(); }
	const Position& operator()(const TernaryExpressionPtr& value) { return value->getPosition(); }
	const Position& operator()(const MultiplicationPtr& value) { return value->getPosition(); }
	const Position& operator()(const AdditionPtr& value) { return value->getPosition(); }
};

const Position& getExpressionPosition(const Expression& val) {
	return std::visit(ExpressionPositionVisitor(), val);
}