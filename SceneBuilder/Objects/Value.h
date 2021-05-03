#pragma once
#include <variant>
#include "Color.h"
#include "DecimalValue.h"
#include "HexValue.h"
#include "Point.h"
#include "Identifier.h"

using Value = std::variant<DecimalValue, HexValue, Color, Point, Identifier>;

const Position& getValuePosition(const Value& val);
