#include "Numbers.h"
#include "cmath"
#include <cstdlib>
#include <ctime>
#include "Mathf.h"

using namespace std;

namespace Crawler {
namespace Numbers {

bool Init(){
	srand (static_cast <unsigned> (time(nullptr)));
	return true;
}
int Wrap(int min, int max, int val){
	int range = max - min + 1;
	if (val < min){
		val += range * ((min - val) / range + 1);
	}
	return min + (val - min) % range;
}
float Wrap(float min, float max, float val){
	return min + fmodf(max - min + fmodf(val - min, max - min), max - min);
}
float Mod(float a, float b){
	return fmodf(a, b);
}
int Clamp(int min, int max, int val){
	if(val > max){
		return max;
	} else if(val < min){
		return min;
	}
	return val;
}
float Clamp(float min, float max, float val){
	if(val > max){
		return max;
	} else if(val < min){
		return min;
	}
	return val;
}
int Random(int min, int max){
	return min+static_cast<int>((static_cast<float>(rand())/RAND_MAX*(max-min)));
}
float Random(float min, float max){
	return min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
}
int Max(int a, int b, int c){
	int max = a;
	if(b > a) max = b;
	if(c > b) max = c;
	return max;
}
int Min(int a, int b, int c){
	int min = a;
	if(b < a) min = b;
	if(c < b) min = c;
	return min;
}
float Max(float a, float b){
    if(a > b) return a;
    return b;
}
float Min(float a, float b){
    if(a < b) return a;
    return b;
}
float Max(float a, float b, float c){
	float max = a;
	if(b > a) max = b;
	if(c > b) max = c;
	return max;
}
float Min(float a, float b, float c){
	float min = a;
	if(b < a) min = b;
	if(c < b) min = c;
	return min;
}
float Stepify(float value, float step){
	value = Wrap(0.0f, 360.0f, value+step/2.0f);
	return value - Mod(value, step);
}
float RotationStep(float rotation, int steps){
	if(steps == 0.0f) return rotation;
	float step = PI2f / steps;
	rotation = Numbers::Mod(rotation+step/2.0f, 360.0f);
	return rotation - Numbers::Mod(rotation, step);
}

}
}


