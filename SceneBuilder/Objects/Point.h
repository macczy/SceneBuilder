#pragma once
#include <tuple>
#include <string>
#include "DecimalValue.h"
#include "../Util/Token.h"

using triplePointValues = std::tuple<DecimalValue, DecimalValue, DecimalValue>;

class Point {
public:
	Point(const Position& position, const DecimalValue& x, const DecimalValue& y, const DecimalValue& z) : values(x, y, z), position(position) {};
	Point(const Position& position, const DecimalValue& x, const DecimalValue& y) : Point(position, x, y, DecimalValue(y.getPosition(), "0")) {};
	Point(const Position& position, const DecimalValue& x) : Point(position, x, DecimalValue(x.getPosition(), "0")) {};
	Point(const Position& position) : Point(position, DecimalValue(position, "0"))  {};

	const triplePointValues& getValues() { return values; }
	const Position& getPosition() const { return position; }
private:
	triplePointValues values;
	Position position;
};

