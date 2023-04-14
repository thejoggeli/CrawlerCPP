#pragma once

namespace Crawler {

class Timer {
public:
	float startTime = 0.0f;
	float endTime = 0.0f;
	float duration = 0.0f;
	Timer();
	void Start(float duration);
	void Restart(bool seamless = false);
	void Reset();

	// true after timer finished
	// false after instantiation
	// false after Reset()
	bool IsFinished();

	// true while timer not finished
	// false after instantiation
	// false after Reset()
	bool IsRunning();

	float GetAbsoluteRemainingTime();
	float GetRelativeRemainingTime();
	float GetAbsolutePassedTime();
	float GetRelativePassedTime();
};

}
