#pragma once
#include <memory>
#include <vector>
#include "../Scanner/Token.h"
#include "Animation.h"

class AnimationDeclaration {
public:
	AnimationDeclaration(const Position& pos, const std::string& name, std::vector<Identifier>& args, Properties& properties, Animations& animations)
		: pos(pos), name(name), args(std::move(args)), properties(std::move(properties)), animations(std::move(animations)) {}
	const Position& getPosition() { return pos; }
	const std::string& getName() { return name; }
	std::vector<Identifier>& getArgs() { return args; }
	Properties& getProperties() { return properties; }
	Animations& getAnimations() { return animations; }
private:
	Position pos;
	std::string name;
	std::vector<Identifier> args;
	Properties properties;
	Animations animations;
};

using AnimationDeclarationPtr = std::unique_ptr<AnimationDeclaration>;