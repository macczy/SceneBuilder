#pragma once
#include <vector>
#include "../Scanner/Token.h"
#include "Expression.h"

using Argument = std::vector<Expression>;
using Arguments = std::vector<Argument>;

class AnimationCall {
public:
	AnimationCall(const Position& pos, const std::string& name, Arguments& args) : pos(pos), name(name), args(std::move(args)) {}
	const Position& getPosition() const { return pos; }
	std::string getName() { return name; }
private:
	Position pos;
	std::string name;
	Arguments args;
};
using AnimationCallPtr = std::unique_ptr<AnimationCall>;
