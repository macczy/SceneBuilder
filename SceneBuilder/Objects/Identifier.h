#pragma once
#include <optional>
#include "../Scanner/Token.h"

class Identifier
{
public:
	Identifier(const Position& position, const std::string& value, Identifier next) :
		position(position), value(value), next(std::move(next)) {};
	Identifier(const Position& position, const std::string& value) :
		position(position), value(value) {};

	const Position& getPosition() { return position; }
	bool hasNext() {
		return next.has_value();
	}
private:
	std::string value;
	Position position;
	std::optional<Identifier> next;
};

