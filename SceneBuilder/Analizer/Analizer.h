#pragma once
#include "../Parser/SceneRoot.h"
#include "../Util/ReturnType.h"

class Analizer
{
public:
	Analizer(SceneRootPtr& root);
	ReturnType evaluateExpression(Object& context, Expression& expression);


private:
	struct LogicalExpressionEvaluationVisitor;
	struct ExpressionEvaluationVisitor;
	SceneRootPtr root;
};