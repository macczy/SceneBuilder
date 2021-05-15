#pragma once
#include "../Scanner/Token.h"

class TimeDeclaration {
public:
	enum class timeSpecifier {
		minute,
		second,
		milisecond
	};
	TimeDeclaration(const Position& position, const std::string& timeValue, const std::string& timeSpecifier) : timeValue(timeValue), position(position) {
		if (timeSpecifier == "m") specifier = timeSpecifier::minute;
		else if (timeSpecifier == "s") specifier = timeSpecifier::second;
		else if (timeSpecifier == "ms") specifier = timeSpecifier::milisecond;
		else throw std::runtime_error(" time specifier for time declaration");
	};
	const std::string& getValue() { return timeValue; }
	const timeSpecifier& getTimeSpecifier() { return specifier; }
	const Position& getPosition() const { return position; }
	static inline bool isCorrectTimeSpecifier(const std::string& time) {
		return time == "s" || time == "m" || time == "ms";
	}
private:
	Position position;
	std::string timeValue;
	timeSpecifier specifier;
};