#include <array>
#include "Animation.h"


//std::map<std::string, std::function<AnimationPtr(const Position& pos, Properties& properties, const std::string& name, std::vector<AnimationPtr> subanimations)>> generator{
//	{"Wait", [](const Position& position, Expression& expr1, Expression& expr2) {
//		return std::make_unique<Wait>(position, expr1, expr2);
//	}},
//	{"AnimationSequence", [](const Position& position, Expression& expr1, Expression& expr2) {
//		return std::make_unique<AnimationSequence>(position, expr1, expr2);
//	}},
//	{"ParalelAnimation", [](const Position& position, Expression& expr1, Expression& expr2) {
//		return std::make_unique<ParalelAnimation>(position, expr1, expr2);
//	}},
//	{"Animation", [](const Position& position, Expression& expr1, Expression& expr2) {
//		return std::make_unique<Animation>(position, expr1, expr2);
//	}},
//	{"ConditionalAnimation", [](const Position& position, Expression& expr1, Expression& expr2) {
//		return std::make_unique<ConditionalAnimation>(position, expr1, expr2);
//	}}
//};
//
//std::unique_ptr<Animation> AnimationFactory::getAnimation(const Position& pos, Properties& properties, const std::string& name, std::vector<AnimationPtr> subanimations) {
//	if (!isAnimationType(name)) return nullptr;
//	auto lambda = generator.find(name);
//	if (lambda == generator.end()) return nullptr;
//	return lambda->second(pos, properties, name, subanimations);
//}

std::array<std::string, 5> AnimationFactory::names = {
		"Wait",
		"AnimationSequence",
		"ParalelAnimation",
		"Animation",
		"ConditionalAnimation"
};

bool AnimationFactory::isAnimationType(const std::string& name) {
	auto res = std::find(names.begin(), names.end(), name);
	return res != names.end();
}