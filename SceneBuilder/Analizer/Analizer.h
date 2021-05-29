#pragma once
#include "../Parser/SceneRoot.h"
#include "../Util/ReturnType.h"

class Analizer
{
public:
	Analizer(SceneRoot* root);

	bool isValid();

	ReturnType evaluateExpression(Object& context, Expression& expression);

private:
	struct LogicalExpressionEvaluationVisitor {
		LogicalExpressionEvaluationVisitor(Object& context);
		Object& context;
		void operator()(const LogicalExpressionPtr& value);
		void operator()(const ComparisonPtr& value);
	};
	struct ExpressionEvaluationVisitor {
		ExpressionEvaluationVisitor(Object& context);
		Object& context;
		ReturnType operator()(const DecimalValue& value);
		ReturnType operator()(const Color & value);
		ReturnType operator()(const Point & value);
		ReturnType operator()(const PointArray & value);
		ReturnType operator()(const TimeDeclaration & value);
		ReturnType operator()(const ConstantIdentifier & value);
		ReturnType operator()(const Identifier & value);
		ReturnType operator()(const AnimationProperty & value);
		ReturnType operator()(const TernaryExpressionPtr & value);
		ReturnType operator()(const MultiplicationPtr & value);
		ReturnType operator()(const AdditionPtr & value);
	};
	SceneRoot* root;
	std::map<std::string, ReturnType> buildInPropertiesTypes;
};