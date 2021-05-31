#pragma once
#include <vector>
#include "Expression.h"

class PointArray {
public:
	PointArray(const Position& pos, std::vector<Point>& value) : pos(pos), value(std::move(value)) {}
	const Position& getPosition() const { return pos; }
	const std::vector<Point>& getArray() const { return value; }
private:
	Position pos;
	std::vector<Point> value;
};