#pragma once
#include <vector>
#include "../Util/Token.h"
#include "AnimationCall.h"

class AnimationProperty {
public:
	AnimationProperty(const Position& pos, std::vector<AnimationCallPtr>& anims) : pos(pos), animationCalls(std::move(anims)) {}
	const Position& getPosition() const { return pos; }
	std::vector<AnimationCallPtr>& getAnimationCalls() { return animationCalls; }
private:
	Position pos;
	std::vector<AnimationCallPtr> animationCalls;
};