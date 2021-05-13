#pragma once
#include <memory>
#include "../Scanner/Token.h"
#include "Animation.h"

class AnimationDeclaration {
public:
	AnimationDeclaration(const Position& pos) : pos(pos) {}
	const Position& getPosition() { return pos; }
private:
	Position pos;
};

using AnimationDeclarationPtr = std::unique_ptr<AnimationDeclaration>;