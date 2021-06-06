#include "Analizer.h"
#include "../Exceptions/TypeMismatch.h"
#include "../Exceptions/SyntaxError.h"
#include "../Exceptions/UndefinedObject.h"
#include "../Exceptions/WrongNumberOfArguments.h"

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
		{ "position", ReturnType::POINT },

	};
}

void Analizer::validateAnimationProperty(const AnimationProperty& animation)
{
	auto& animationCalls = animation.getAnimationCalls();
	for (auto& animationCall : animationCalls)
	{
		bool found = false;
		for (auto& declaration : root->getKnownAnimations())
		{
			if (declaration->getName() == animationCall->getName())
			{
				if (declaration->getArgs().size() != animationCall->getArguments().size())
				{
					throw WrongNumberOfArguments((int)animationCall->getArguments().size(), (int)declaration->getArgs().size(), animationCall->getName(), animationCall->getPosition());
				}
				found = true;
				break;
			}
		}
		if (!found)
			throw UndefinedObject(animationCall->getName(), animationCall->getPosition());
	}
}

void Analizer::validateObjectProperties(const std::string& typeName, Properties& properties)
{
	for (auto& prop : properties) {
		if (prop->getName() == "animation") {
			if (!std::holds_alternative<AnimationProperty>(prop->getValue())) {
				throw SyntaxError("animation property", "as seen below", prop->getPosition());
			}
			auto& animationProperty = std::get<AnimationProperty>(prop->getValue());
			validateAnimationProperty(animationProperty);
		}
		else { //TODO


		}
	}
}

#include <iostream>

void Analizer::validateObjects(const Objects& objects) {
	for (auto& object : objects)
	{
		auto type = object->object->getObjectTypeName();
		auto& position = object->pos;

		validateObjectProperties(type, object->object->getProperties());
		 
		if (type == "Circle" || type == "Line" || type == "Rectangle" || type == "Polygon")
			continue;

		//all used objects must be declared above to easily avoid circular dependencies
		auto declaredType = std::find_if(root->getKnownObjects().begin(), root->getKnownObjects().end(),
			[type, position](const ComplexObjectDeclarationPtr& obj) {
			return obj->getName() == type && (position.lineNumber > obj->getPosition().lineNumber || ((position.lineNumber == obj->getPosition().lineNumber) && position.columnNumber == obj->getPosition().columnNumber)); });
		if (declaredType == root->getKnownObjects().end()) {
			throw UndefinedObject(type, object->pos);
		}
	}
}


//TODO
bool Analizer::isValid()
{
	if (!root) return false;

	if (!root->getScene()) return false;

	for (auto& obj : root->getKnownObjects()) {
		validateObjects(obj->getObjects());
	}

	validateObjects(root->getScene()->getObjects());

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