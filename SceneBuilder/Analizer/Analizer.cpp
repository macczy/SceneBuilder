#include "Analizer.h"
#include "../Exceptions/TypeMismatch.h"

Analizer::Analizer(SceneRootPtr& root) : root(std::move(root)) {}

//throws if subexpressions types mismatch
struct Analizer::LogicalExpressionEvaluationVisitor {
	void operator()(const LogicalExpressionPtr& value) {
		auto first = value->getFirstValue();
		std::visit(LogicalExpressionEvaluationVisitor(), *first);
		auto second = value->getSecondValue();
		std::visit(LogicalExpressionEvaluationVisitor(), *second);
	}
	void operator()(const ComparisonPtr& value) {
		auto& first = value->getFirstExpression();
		auto firstReturnType = std::visit(ExpressionEvaluationVisitor(), first);
		auto& second = value->getSecondExpression();
		auto secondReturnType = std::visit(ExpressionEvaluationVisitor(), second);
		if (firstReturnType != secondReturnType) {
			throw TypeMismatch(toString(firstReturnType), toString(secondReturnType), value->getPosition());
		}
		//some types can only be compared with equals or not equals
		if (firstReturnType == ReturnType::BUILD_IN_CONSTANT
				|| firstReturnType == ReturnType::POINT_ARRAY
				|| firstReturnType == ReturnType::NON_TYPE
				|| firstReturnType == ReturnType::COLOR
				|| firstReturnType == ReturnType::POINT) {
			auto equals = dynamic_cast<Equal*>(value.get());
			auto notequals = dynamic_cast<NotEqual*>(value.get());
			if(equals || notequals )
				throw TypeMismatch(toString(firstReturnType), toString(secondReturnType), value->getPosition());
		}
	}
};

struct Analizer::ExpressionEvaluationVisitor {
	ReturnType operator()(const DecimalValue& value) { return ReturnType::DECIMAL_VALUE; }
	ReturnType operator()(const Color& value) { return ReturnType::COLOR; }
	ReturnType operator()(const Point& value) { return ReturnType::POINT; }
	ReturnType operator()(const PointArray& value) { return ReturnType::POINT_ARRAY; }
	ReturnType operator()(const TimeDeclaration& value) { return ReturnType::TIME_DECLARATION; }
	ReturnType operator()(const ConstantIdentifier& value) { return ReturnType::BUILD_IN_CONSTANT; }

	ReturnType operator()(const Identifier& value) { throw std::runtime_error("not implemented"); }

	ReturnType operator()(const AnimationProperty& value) { throw std::runtime_error("not implemented"); }

	ReturnType operator()(const TernaryExpressionPtr& value) { 
		std::visit(LogicalExpressionEvaluationVisitor(), *value->getCondition());
		auto& first = value->getTrueExpression();
		auto firstReturnType = std::visit(ExpressionEvaluationVisitor(), first);
		auto& second = value->getTrueExpression();
		auto secondReturnType = std::visit(ExpressionEvaluationVisitor(), second);
		if (firstReturnType != secondReturnType) {
			throw TypeMismatch(toString(firstReturnType), toString(secondReturnType), value->getPosition());
		}
		return firstReturnType;
	}
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