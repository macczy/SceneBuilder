#include "Analizer.h"
#include "../Exceptions/TypeMismatch.h"

Analizer::Analizer(SceneRoot* root) : root(root) {
	buildInPropertiesTypes = {
		{ "width", ReturnType::DECIMAL_VALUE },
		{ "height", ReturnType::DECIMAL_VALUE },
		{ "x", ReturnType::DECIMAL_VALUE },
		{ "y", ReturnType::DECIMAL_VALUE },
		{ "z", ReturnType::DECIMAL_VALUE },
		{ "radius", ReturnType::DECIMAL_VALUE },
		{ "color", ReturnType::COLOR },
		{ "duration", ReturnType::TIME_DECLARATION },
		{ "vertexes", ReturnType::POINT_ARRAY },
		{ "vertex", ReturnType::POINT },

	};
}

//TODO
bool Analizer::isValid()
{
	return true;
}

Analizer::LogicalExpressionEvaluationVisitor::LogicalExpressionEvaluationVisitor(Object& context) : context(context) {}

void Analizer::LogicalExpressionEvaluationVisitor::operator()(const LogicalExpressionPtr& value) {
	auto first = value->getFirstValue();
	std::visit(LogicalExpressionEvaluationVisitor(context), *first);
	auto second = value->getSecondValue();
	std::visit(LogicalExpressionEvaluationVisitor(context), *second);
}
void Analizer::LogicalExpressionEvaluationVisitor::operator()(const ComparisonPtr& value) {
	auto& first = value->getFirstExpression();
	auto firstReturnType = std::visit(ExpressionEvaluationVisitor(context), first);
	auto& second = value->getSecondExpression();
	auto secondReturnType = std::visit(ExpressionEvaluationVisitor(context), second);
	if (firstReturnType != secondReturnType) {
		throw TypeMismatch(firstReturnType, secondReturnType, value->getPosition());
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
			throw TypeMismatch(firstReturnType, secondReturnType, value->getPosition());
	}
}

Analizer::ExpressionEvaluationVisitor::ExpressionEvaluationVisitor(Object& context) : context(context) {}
ReturnType Analizer::ExpressionEvaluationVisitor::operator()(const DecimalValue& value) { return ReturnType::DECIMAL_VALUE; }
ReturnType Analizer::ExpressionEvaluationVisitor::operator()(const Color& value) { return ReturnType::COLOR; }
ReturnType Analizer::ExpressionEvaluationVisitor::operator()(const Point& value) { return ReturnType::POINT; }
ReturnType Analizer::ExpressionEvaluationVisitor::operator()(const PointArray& value) { return ReturnType::POINT_ARRAY; }
ReturnType Analizer::ExpressionEvaluationVisitor::operator()(const TimeDeclaration& value) { return ReturnType::TIME_DECLARATION; }
ReturnType Analizer::ExpressionEvaluationVisitor::operator()(const ConstantIdentifier& value) { return ReturnType::BUILD_IN_CONSTANT; }

ReturnType Analizer::ExpressionEvaluationVisitor::operator()(const Identifier& value) { throw std::runtime_error("not implemented"); }

ReturnType Analizer::ExpressionEvaluationVisitor::operator()(const AnimationProperty& value) { throw std::runtime_error("not implemented"); }

ReturnType Analizer::ExpressionEvaluationVisitor::operator()(const TernaryExpressionPtr& value) {
	std::visit(LogicalExpressionEvaluationVisitor(context), *value->getCondition());
	auto& first = value->getTrueExpression();
	auto firstReturnType = std::visit(ExpressionEvaluationVisitor(context), first);
	auto& second = value->getTrueExpression();
	auto secondReturnType = std::visit(ExpressionEvaluationVisitor(context), second);
	if (firstReturnType != secondReturnType) {
		throw TypeMismatch(firstReturnType, secondReturnType, value->getPosition());
	}
	return firstReturnType;
}
//MultiplicationPtr or AdditionPtr
ReturnType Analizer::ExpressionEvaluationVisitor::operator()(const MultiplicationPtr& value) {
	auto& first = value->getFirstExpression();
	auto firstReturnType = std::visit(ExpressionEvaluationVisitor(context), first);
	auto& second = value->getSecondExpression();
	auto secondReturnType = std::visit(ExpressionEvaluationVisitor(context), second);
	auto returnType = value->getReturnType(firstReturnType, secondReturnType);
	if (returnType == ReturnType::NON_TYPE) {
		throw TypeMismatch(firstReturnType, secondReturnType, value->getPosition());
	}
	return returnType;
}
ReturnType Analizer::ExpressionEvaluationVisitor::operator()(const AdditionPtr& value) {
	auto& first = value->getFirstExpression();
	auto firstReturnType = std::visit(ExpressionEvaluationVisitor(context), first);
	auto& second = value->getSecondExpression();
	auto secondReturnType = std::visit(ExpressionEvaluationVisitor(context), second);
	auto returnType = value->getReturnType(firstReturnType, secondReturnType);
	if (returnType == ReturnType::NON_TYPE) {
		throw TypeMismatch(firstReturnType, secondReturnType, value->getPosition());
	}
	return returnType;
}

ReturnType Analizer::evaluateExpression(Object& context, Expression& expression) {
	return std::visit(ExpressionEvaluationVisitor(context), expression);
} 