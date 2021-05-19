#pragma once
#include <memory>
#include "../Scanner/Token.h"

class Identifier;

using IdentifierPtr = std::unique_ptr<Identifier>;
class Identifier {
public:
	Identifier(const Position& position, const std::string& value, std::unique_ptr<Identifier>& next) :
		position(position), value(value), next(std::move(next)) {};
	Identifier(const Position& position, const std::string& value) :
		position(position), value(value) {};

	const Position& getPosition() const { 
		return position; 
	}
	bool hasNext() const {
		return (bool)next;
	}
	const std::string& getValue() const {
		return value;
	}
	const Identifier* getNext() const {
		return next.get();
	}
private:
	std::string value;
	Position position;
	IdentifierPtr next;
};

