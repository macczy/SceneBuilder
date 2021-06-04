#pragma once
#include <vector>
#include "../Util/Token.h"
#include "Expression.h"

using Argument = std::vector<Expression>;
using Arguments = std::vector<Argument>;

class AnimationCall {
public:
	AnimationCall(const Position& pos, const std::string& name, Arguments& args) : pos(pos), name(name), args(std::move(args)) {}
	const Position& getPosition() const { return pos; }
	const std::string getName() const { return name; }
	const Arguments& getArguments() const { return args; }
private:
	Position pos;
	std::string name;
	Arguments args;
};
using AnimationCallPtr = std::unique_ptr<AnimationCall>;

