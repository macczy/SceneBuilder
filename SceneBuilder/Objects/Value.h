#pragma once
#include <variant>
#include "Color.h"
#include "DecimalValue.h"
#include "HexValue.h"
#include "Point.h"
#include "Identifier.h"

using Value = std::variant<DecimalValue, HexValue, Color, Point, Identifier>;

namespace VALUE {
	const std::size_t DECIMAL_VALUE_INDEX = 0;
	const std::size_t HEX_VALUE_INDEX = 1;
	const std::size_t COLOR_INDEX = 2;
	const std::size_t POINT_INDEX = 3;
	const std::size_t IDENTIFIER_INDEX = 4;
}

const Position& getValuePosition(Value& val);