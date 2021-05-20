#include "Analizer.h"
#include "../Exceptions/TypeMismatch.h"

Analizer::Analizer(SceneRootPtr& root) : root(std::move(root)) {}

struct ExpressionEvaluationVisitor {
	ReturnType operator()(const DecimalValue& value) { return ReturnType::DECIMAL_VALUE; }
	ReturnType operator()(const Color& value) { return ReturnType::COLOR; }
	ReturnType operator()(const Point& value) { return ReturnType::POINT; }
	ReturnType operator()(const PointArray& value) { return ReturnType::POINT_ARRAY; }
	ReturnType operator()(const TimeDeclaration& value) { return ReturnType::TIME_DECLARATION; }

	ReturnType operator()(const Identifier& value) { throw std::runtime_error("not implemented"); }
	ReturnType operator()(const ConstantIdentifier& value) { throw std::runtime_error("not implemented"); }
	ReturnType operator()(const AnimationProperty& value) { throw std::runtime_error("not implemented"); }
	ReturnType operator()(const TernaryExpressionPtr& value) { throw std::runtime_error("not implemented"); }
	ReturnType operator()(const MultiplicationPtr& value) { 
		auto& first = value->getFirstExpression();
		auto firstReturnType = std::visit(ExpressionEvaluationVisitor(), first);
		auto& second = value->getSecondExpression();
		auto secondReturnType = std::visit(ExpressionEvaluationVisitor(), second);
		auto returnType = value->getReturnType(firstReturnType, secondReturnType);
		if (returnType == ReturnType::NON_TYPE) {
			throw TypeMismatch(toString(firstReturnType), toString(secondReturnType), value->getPosition());
		}
		return returnType;
	}
	ReturnType operator()(const AdditionPtr& value) {
		auto& first = value->getFirstExpression();
		auto firstReturnType = std::visit(ExpressionEvaluationVisitor(), first);
		auto& second = value->getSecondExpression();
		auto secondReturnType = std::visit(ExpressionEvaluationVisitor(), second);
		auto returnType = value->getReturnType(firstReturnType, secondReturnType);
		if (returnType == ReturnType::NON_TYPE) {
			throw TypeMismatch(toString(firstReturnType), toString(secondReturnType), value->getPosition());
		}
		return returnType;
	}
};

ReturnType Analizer::evaluateExpression(Object& context, Expression& expression) {
	return std::visit(ExpressionEvaluationVisitor(), expression);
} 