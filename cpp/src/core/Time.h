#pragma once

#include <ctime>
#include <stdint.h>

namespace Crawler {

class Time {
public:

	Time(){}

	static uint64_t fixedDeltaTimeMicros;
	static float fixedDeltaTime;
	
	static uint64_t deltaTimeMicros;
	static uint64_t currentTimeMicros;
	static uint64_t lastTimeMicros; 
	static uint64_t startTimeMicros;

	static float deltaTime;
	static float currentTime;
	static float lastTime;
	static float startTime;

	static void SetFixedDeltaTimeMicros(uint64_t micros);

	static void Start();
	static void Update();

	static float GetTime();
	static uint64_t GetTimeMicros();
	static uint64_t GetSystemTimeMicros();

	static void Sleep(float seconds);
	static void SleepMicros(uint64_t micros);

};

}
