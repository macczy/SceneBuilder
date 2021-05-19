#pragma once
#include <memory>
#include "../Scanner/Token.h"

class ConstantIdentifier;
using ConstantIdentifierPtr = std::unique_ptr<ConstantIdentifier>;

class ConstantIdentifier {
public:
	ConstantIdentifier(const Position& position, const std::string& value, ConstantIdentifierPtr& next) :
		position(position), value(value), next(std::move(next)) {};
	ConstantIdentifier(const Position& position, const std::string& value) :
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
	const ConstantIdentifier* getNext() const {
		return next.get();
	}
private:
	std::string value;
	Position position;
	ConstantIdentifierPtr next;
};

