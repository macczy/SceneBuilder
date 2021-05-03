#include "Value.h"

const Position& getValuePosition(const Value& val) {
	return std::visit([](auto&& value) { return value.getPosition(); }, val);
}