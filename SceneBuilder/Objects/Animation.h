#pragma once
#include <memory>
#include "../Scanner/Token.h"
#include "Property.h"

class Animation
{
public:
	Animation(const Position& pos, Properties& properties) : pos(pos), properties(std::move(properties)) {}
	virtual ~Animation() {}
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
	AnimationSequence(const Position& pos, Properties& properties) : Animation(pos, properties) {}
	virtual ~AnimationSequence() {}
};

class ConditionalAnimation : public Animation
{
public:
	ConditionalAnimation(const Position& pos, Properties& properties) : Animation(pos, properties) {}
	virtual ~ConditionalAnimation() {}
};

class ParalelAnimation : public Animation
{
public:
	ParalelAnimation(const Position& pos, Properties& properties) : Animation(pos, properties) {}
	virtual ~ParalelAnimation() {}
};