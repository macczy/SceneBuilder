
#include "pch.h"
#include "../SceneBuilder/Analizer/Analizer.cpp"
#include "../SceneBuilder/Objects/ComplexObject.cpp"


/*
enum class ReturnType {
	DECIMAL_VALUE,
	POINT,
	POINT_ARRAY,
	COLOR,
	BOOLEAN,
	TIME_DECLARATION,
	BUILD_IN_CONSTANT
};
*/

TEST(AnalizerUnitTests, defaultConstructorTest) {
	SceneRootPtr pt;
	Analizer analizer(pt);
}


TEST(AnalizerUnitTests, evaluatePoint) {
	SceneRootPtr pt;
	Analizer analizer(pt);
	Expression expr{ Point{ Position{}} };
	Circle context{ Position(), Properties() };
	if (analizer.evaluateExpression(context, expr) != ReturnType::POINT) {
		FAIL() << "POINT type expected";
	}
}

TEST(AnalizerUnitTests, evaluateDecimalValue) {
	SceneRootPtr pt;
	Analizer analizer(pt);
	Expression expr{ DecimalValue{Position{}, "10.01"} };
	Circle context{ Position(), Properties() };
	if (analizer.evaluateExpression(context, expr) != ReturnType::DECIMAL_VALUE) {
		FAIL() << "DECIMAL VALUE type expected";
	}
}

TEST(AnalizerUnitTests, evaluateColor) {
	SceneRootPtr pt;
	Analizer analizer(pt);
	Expression expr{ Color{Position{}, "", "", "" } };
	Circle context{ Position(), Properties() };
	if (analizer.evaluateExpression(context, expr) != ReturnType::COLOR) {
		FAIL() << "DECIMAL VALUE type expected";
	}
}