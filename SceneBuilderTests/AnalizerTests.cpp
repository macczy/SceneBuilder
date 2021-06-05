#include "pch.h"

TEST(AnalizerUnitTests, defaultConstructorTest) {
	SceneRootPtr pt;
	Analizer analizer(pt.get());
}

TEST(AnalizerUnitTests, evaluatePoint) {
	SceneRootPtr pt;
	Analizer analizer(pt.get());
	Expression expr{ Point{ Position{}} };
	Circle context{ Position(), Properties() };
	if (analizer.evaluateExpression(context, expr) != ReturnType::POINT) {
		FAIL() << "POINT type expected";
	}
}

TEST(AnalizerUnitTests, evaluateDecimalValue) {
	SceneRootPtr pt;
	Analizer analizer(pt.get());
	Expression expr{ DecimalValue{Position{}, "10.01"} };
	Circle context{ Position(), Properties() };//dummy context
	if (analizer.evaluateExpression(context, expr) != ReturnType::DECIMAL_VALUE) {
		FAIL() << "DECIMAL VALUE type expected";
	}
}

TEST(AnalizerUnitTests, evaluateColor) {
	SceneRootPtr pt;
	Analizer analizer(pt.get());
	Expression expr{ Color{Position{}, "", "", "" } };
	Circle context{ Position(), Properties() };//dummy context
	if (analizer.evaluateExpression(context, expr) != ReturnType::COLOR) {
		FAIL() << "DECIMAL VALUE type expected";
	}
}
TEST(AnalizerUnitTests, evaluateTimeDeclaration) {
	SceneRootPtr pt;
	Analizer analizer(pt.get());
	Expression expr{ TimeDeclaration{Position{}, "10", "s"} };
	Circle context{ Position(), Properties() }; //dummy context
	if (analizer.evaluateExpression(context, expr) != ReturnType::TIME_DECLARATION) {
		FAIL() << "TIME DECLARATION type expected";
	}
}

TEST(AnalizerUnitTests, evaluateAddition) {
	SceneRootPtr pt;
	Analizer analizer(pt.get());
	{
		Expression expr1{ TimeDeclaration{Position{}, "10", "s"} };
		Expression expr2{ TimeDeclaration{Position{}, "10", "s"} };
		Expression expr{ std::make_unique<Sum>(Position(), expr1, expr2) };
		Circle context{ Position(), Properties() }; //dummy context
		if (analizer.evaluateExpression(context, expr) != ReturnType::TIME_DECLARATION) {
			FAIL() << "TIME DECLARATION type expected";
		}
	}
	{
		Expression expr1{ TimeDeclaration{Position{}, "10", "s"} };
		Expression expr2{ Color{Position{}, "xA2", "xB", "x12"} };
		Expression expr{ std::make_unique<Substraction>(Position(), expr1, expr2) };
		Circle context{ Position(), Properties() }; //dummy context
		try {
			if (analizer.evaluateExpression(context, expr) != ReturnType::NON_TYPE) {
				FAIL() << "Type mismatch expected";
			}
		}
		catch (const TypeMismatch& err) {
			EXPECT_EQ(err.first, toString(ReturnType::TIME_DECLARATION));
			EXPECT_EQ(err.second, toString(ReturnType::COLOR));
		}
		catch (...) {
			FAIL() << "Type mismatch expected";
		}
	}
}


TEST(AnalizerUnitTests, evaluateMultiplication) {
	SceneRootPtr pt;
	Analizer analizer(pt.get());
	{
		Expression expr1{ DecimalValue{Position{}, "10"} };
		Expression expr2{ TimeDeclaration{Position{}, "10", "s"} };
		Expression expr{ std::make_unique<Multiplication_>(Position(), expr1, expr2) };
		Circle context{ Position(), Properties() }; //dummy context
		if (analizer.evaluateExpression(context, expr) != ReturnType::TIME_DECLARATION) {
			FAIL() << "TIME DECLARATION type expected";
		}
	}
	{
		Expression expr1{ DecimalValue{Position{}, "10"} };
		Expression expr2{ TimeDeclaration{Position{}, "10", "s"} };
		Expression expr{ std::make_unique<Division>(Position(), expr1, expr2) };
		Circle context{ Position(), Properties() }; //dummy context
		try {
			if (analizer.evaluateExpression(context, expr) != ReturnType::NON_TYPE) {
				FAIL() << "Type mismatch expected";
			}
		}
		catch (const TypeMismatch& err) {
			EXPECT_EQ(err.first, toString(ReturnType::DECIMAL_VALUE));
			EXPECT_EQ(err.second, toString(ReturnType::TIME_DECLARATION));
		}
		catch (...) {
			FAIL() << "Type mismatch expected";
		}
	}
}