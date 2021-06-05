#pragma once

class Animation {
public:
	Animation() : state(0), totalTime(0) {}
protected:
	int state;
	float totalTime;
};
