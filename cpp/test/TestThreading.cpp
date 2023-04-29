#include <iostream>
#include "core/Time.h"
#include "core/Log.h"
#include <thread>
#include "threading/Signal.h"
#include "math/Numbers.h"

// #include <Eigen/Dense>

using namespace Crawler;
using namespace std;

Signal s1;
Signal s2;
Signal s3;
Signal s4;

void func1(){
    int runs = 0;
    while(true){
        Time::Sleep(Numbers::Random(0.1f, 1.0f));
        LogInfo("fun1", "set s1");
        s1.Set();
        Time::Sleep(Numbers::Random(0.1f, 1.0f));
        LogInfo("fun1", "wait s3");
        s3.WaitAndClear();
        runs++;
        LogInfo("fun1", iLog << "runs=" << runs);        
    }
}

void func2(){
    int runs = 0;
    while(true){
        Time::Sleep(Numbers::Random(0.1f, 1.0f));
        LogInfo("fun2", "set s2");
        s2.Set();
        Time::Sleep(Numbers::Random(0.1f, 1.0f));
        LogInfo("fun2", "wait s4");
        s4.WaitAndClear();
        runs++;
        LogInfo("fun2", iLog << "runs=" << runs);        
    }
}


int main(){

    Numbers::Init();

    thread t1 = thread(&func1);
    thread t2 = thread(&func2);

    t1.detach();
    t2.detach();

    int runs = 0;

    while(true){
        
        Time::Sleep(Numbers::Random(0.1f, 1.0f));
        LogInfo("main", "wait s1");
        s1.WaitAndClear();

        Time::Sleep(Numbers::Random(0.1f, 1.0f));
        LogInfo("main", "wait s2");
        s2.WaitAndClear();

        Time::Sleep(Numbers::Random(0.1f, 1.0f));
        LogInfo("main", "set s3");
        s3.Set();

        Time::Sleep(Numbers::Random(0.1f, 1.0f));
        LogInfo("main", "set s4");
        s4.Set();

        runs++;
        LogInfo("main", iLog << "runs=" << runs);        

    }

    // done
    return EXIT_SUCCESS;

}