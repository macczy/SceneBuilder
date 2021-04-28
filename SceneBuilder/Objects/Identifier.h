#pragma once
#include <memory>
#include "../Scanner/Token.h"

class Identifier
{
public:
	Identifier(const Position& position, const std::string& value, std::unique_ptr<Identifier> next) :
		position(position), value(value), next(std::move(next)) {};
	Identifier(const Position& position, const std::string& value) :
		position(position), value(value) {};

	const Position& getPosition() { return position; }
	bool hasNext() {
		return (bool)next;
	}
private:
	std::string value;
	Position position;
	std::unique_ptr<Identifier> next;
};

