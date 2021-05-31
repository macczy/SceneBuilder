#pragma once
#include "../Objects/Expression.h"
#include "../Parser/SceneRoot.h"


class Generator
{
public:
	Generator(SceneRootPtr&& root);
	bool generate(const std::string& filename);
	void run();
	std::string generateExpression(const Expression& expr);
	std::string getMembers(Objects& objects);
	std::string getGetters(Objects& objects);
	std::string generateSubObjectInitialization(ObjectIdentifierPairPtr& object);
	std::string getClassDeclaration(ComplexObjectDeclarationPtr& objectDeclaration);
	std::string getIncludeStatements(Objects& objectDeclaration);
	std::string generateSceneClass(Scene* scene);




	struct LogicalExpressionGeneratorVisitor {
		std::string operator()(const LogicalExpressionPtr& value);
		std::string operator()(const ComparisonPtr& value);
	};

	struct ExpressionGeneratorVisitor {
		std::string operator()(const DecimalValue& value);
		std::string operator()(const Color& value);
		std::string operator()(const Point& value);
		std::string operator()(const PointArray& value);
		std::string operator()(const TimeDeclaration& value);
		std::string operator()(const ConstantIdentifier& value);
		std::string operator()(const Identifier& value);
		std::string operator()(const AnimationProperty& value);
		std::string operator()(const TernaryExpressionPtr& value);
		std::string operator()(const MultiplicationPtr& value);
		std::string operator()(const AdditionPtr& value);
	};



private:
	SceneRootPtr root;
};

