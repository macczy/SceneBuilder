#pragma once

class Animation {
public:
	virtual ~Animation() {}
	Animation() : state(0), totalTime(0) {}
protected:
	int state;
	float totalTime;
};
