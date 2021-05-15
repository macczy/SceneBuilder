#pragma once
#include <memory>
#include "../Scanner/Token.h"
#include "Property.h"

class Animation
{
public:
	Animation(const Position& pos, Properties& properties) : pos(pos), properties(std::move(properties)) {}
	virtual ~Animation() {}
	const Properties& getProperties() const { return properties; }
protected:
	Position pos;
	Properties properties;
};

using AnimationPtr = std::unique_ptr<Animation>;

class Wait : public Animation
{
public:
	Wait(const Position& pos, Properties& properties) : Animation(pos, properties) {}
	virtual ~Wait() {}
};

class AnimationSequence : public Animation
{
public:
	AnimationSequence(const Position& pos, Properties& properties, std::vector<AnimationPtr>& animations)
		: Animation(pos, properties), animations(std::move(animations)) {}
	virtual ~AnimationSequence() {}
	const std::vector<AnimationPtr>& getAnimations() { return animations;  }
private:
	std::vector<AnimationPtr> animations;
};

class ConditionalAnimation : public Animation
{
public:
	ConditionalAnimation(const Position& pos, Properties& properties, std::vector<AnimationPtr>& animations, LogicalSubExpressionPtr& condition)
		: Animation(pos, properties), animations(std::move(animations)), condition(std::move(condition)) {}
	virtual ~ConditionalAnimation() {}
	const LogicalSubExpressionPtr& getCondition() { return condition; }
	const std::vector<AnimationPtr>& getAnimations() { return animations; }
private:
	std::vector<AnimationPtr> animations;
	LogicalSubExpressionPtr condition;
};

class ParalelAnimation : public Animation
{
public:
	ParalelAnimation(const Position& pos, Properties& properties, std::vector<AnimationPtr>& animations)
		: Animation(pos, properties), animations(std::move(animations)) {}
	virtual ~ParalelAnimation() {}
	std::vector<AnimationPtr>& getAnimations() { return animations; }
private:
	std::vector<AnimationPtr> animations;
};

class AnimationFactory {
	static std::array<std::string, 5> names;
public:
	static bool isAnimationType(const std::string& name);
};