#include <iostream>
#include "core/Time.h"

// #include <Eigen/Dense>

using namespace Crawler;
using namespace std;

int main(){

    Time::Start();

    cout << "GetSystemTimeMicros: " << Time::GetSystemTimeMicros() << endl;
    cout << "GetTimeMicros: " << Time::GetTimeMicros() << endl;

    uint64_t t_prev = Time::GetTimeMicros();
    while(Time::GetTime() < 5.0f){
        Time::Update();
        uint64_t t_curr = Time::currentTimeMicros;
        if(t_curr < t_prev){
            cout << "ERROR: t_curr < t_prev" << endl;
            return EXIT_FAILURE;
        }
        t_prev = t_curr;
        cout << t_curr << " / " << Time::deltaTime << " / " << Time::deltaTimeMicros << " / " << Time::currentTime << " / " << Time::currentTimeMicros << endl;
    }

    // done
    return EXIT_SUCCESS;

}