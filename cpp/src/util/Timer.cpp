#include "Timer.h"
#include "core/Time.h"

namespace Crawler {

Timer::Timer() {}

void Timer::Start(float duration){
	this->duration = duration;
	startTime = Time::currentTime;
	endTime = startTime + duration;
}
void Timer::Restart(bool seamless){
	if(seamless){
		startTime += duration;
		endTime += duration;
	} else {
		Start(duration);
	}
}
void Timer::Reset(){
	startTime = 0.0f;
	endTime = 0.0f;
	duration = 0.0f;
}
bool Timer::IsFinished(){
	if(duration == 0.0f) return false; // timer is reset
	if(Time::currentTime < endTime) return false; // timer is running
	return true;
}
bool Timer::IsRunning(){
	if(duration == 0.0f) return false; // timer is reset
	if(Time::currentTime < endTime) return true; // timer is running
	return false;
}
float Timer::GetAbsoluteRemainingTime(){
	float remaining = endTime - Time::currentTime;
	return remaining <= 0.0f ? 0.0f : remaining;
}
float Timer::GetRelativeRemainingTime(){
	float remaining = endTime - Time::currentTime;
	return remaining <= 0.0f ? 0.0f : remaining/duration;
}
float Timer::GetAbsolutePassedTime(){
	float passed = Time::currentTime - startTime;
	return passed >= duration ? duration : passed;
}
float Timer::GetRelativePassedTime(){
	float passed = Time::currentTime - startTime;
	return passed >= duration ? duration : passed/duration;
}

}
