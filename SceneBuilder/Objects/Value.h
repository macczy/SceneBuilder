#pragma once
#include <variant>
#include "Color.h"
#include "DecimalValue.h"
#include "HexValue.h"
#include "Point.h"
#include "Identifier.h"


using value = std::variant<DecimalValue, HexValue, Color, Point>;

namespace Value {
	const std::size_t DECIMAL_VALUE_INDEX = 0;
	const std::size_t HEX_VALUE_INDEX = 1;
	const std::size_t COLOR_INDEX = 2;
	const std::size_t POINT_INDEX = 3;
}