#pragma once
#include <ostream>

enum class ReturnType {
	NON_TYPE,
	DECIMAL_VALUE,
	POINT,
	POINT_ARRAY,
	COLOR,
	TIME_DECLARATION,
	BUILD_IN_CONSTANT
};

std::ostream& operator<< (std::ostream& stream, const ReturnType& type);

std::string toString(const ReturnType& type);