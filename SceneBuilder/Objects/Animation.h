#pragma once
#include <memory>
#include "Property.h"

class Animation {
public:
	Animation(const Position& pos, Properties& properties) : pos(pos), properties(std::move(properties)) {}
	virtual ~Animation() {}
	const Properties& getProperties() const { return properties; }
	const Position& getPosition() const { return pos; }
protected:
	Position pos;
	Properties properties;
};

using AnimationPtr = std::unique_ptr<Animation>;
using Animations = std::vector<AnimationPtr>;

class Wait : public Animation {
public:
	Wait(const Position& pos, Properties& properties) : Animation(pos, properties) {}
	virtual ~Wait() {}
};

class AnimationSequence : public Animation {
public:
	AnimationSequence(const Position& pos, Properties& properties, Animations& animations)
		: Animation(pos, properties), animations(std::move(animations)) {}
	virtual ~AnimationSequence() {}
	const Animations& getAnimations() { return animations;  }
	void setIndex(int index) {
		this->index = index;
	}
	int getIndex() { return index;  }
private:
	Animations animations;
	int index = -1;
};

class ConditionalAnimation : public Animation {
public:
	ConditionalAnimation(const Position& pos, Properties& properties, Animations& animations, LogicalSubExpressionPtr& condition)
		: Animation(pos, properties), animations(std::move(animations)), condition(std::move(condition)) {}
	virtual ~ConditionalAnimation() {}
	const LogicalSubExpressionPtr& getCondition() { return condition; }
	const Animations& getAnimations() { return animations; }	
	void setIndex(int index) {
		this->index = index;
	}
	int getIndex() { return index; }
private:
	Animations animations;
	LogicalSubExpressionPtr condition;
	int index = -1;
};

class ParalelAnimation : public Animation {
public:
	ParalelAnimation(const Position& pos, Properties& properties, Animations& animations)
		: Animation(pos, properties), animations(std::move(animations)) {}
	virtual ~ParalelAnimation() {}
	Animations& getAnimations() { return animations; }
private:
	Animations animations;
};