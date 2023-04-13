#include "Time.h"
#include <time.h>
#include <iostream>
#include <math.h>
#include <unistd.h>

namespace Crawler {


uint64_t Time::fixedDeltaTimeMicros = 0;
float Time::fixedDeltaTime = 0.0f;

uint64_t Time::deltaTimeMicros = 0;
uint64_t Time::currentTimeMicros = 0;
uint64_t Time::lastTimeMicros = 0;
uint64_t Time::startTimeMicros = 0;

float Time::deltaTime = 0.0f;
float Time::currentTime = 0.0f;
float Time::lastTime = 0.0f;
float Time::startTime = 0.0f;

void Time::Start(){
	// time offset
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);


	// start
	startTimeMicros = GetSystemTimeMicros();
	currentTimeMicros = 0;
	lastTimeMicros = 0;

	startTime = startTimeMicros * 1.0e-6f;
	currentTime = 0.0f;
	lastTime = 0.0f;

}

void Time::Update(){
	
	lastTimeMicros = currentTimeMicros;
	currentTimeMicros = GetTimeMicros();
	deltaTimeMicros = currentTimeMicros - lastTimeMicros;

	lastTime = lastTimeMicros * 1.0e-6f;
	currentTime = currentTimeMicros * 1.0e-6f;
	deltaTime = deltaTimeMicros * 1.0e-6f;

}

float Time::GetTime(){
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	return GetTimeMicros() * 1.0e-6;
}

uint64_t Time::GetTimeMicros(){
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	return Time::GetSystemTimeMicros() - startTimeMicros;
}

uint64_t Time::GetSystemTimeMicros(){
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	return static_cast<uint64_t>(spec.tv_sec) * static_cast<uint64_t>(1000000) + static_cast<uint64_t>(spec.tv_nsec/1000);
}


void Time::Sleep(float seconds){
	usleep(seconds*1000000.0f);
}

void Time::SleepMicros(uint64_t micros){
	usleep(micros);
}

void Time::SetFixedDeltaTimeMicros(uint64_t micros){
	Time::fixedDeltaTimeMicros = micros;
	Time::fixedDeltaTime = micros * 1.0e-6f;
}

}

