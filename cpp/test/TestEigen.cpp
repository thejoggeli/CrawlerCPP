#include "Eigen/Fix"
#include "Eigen/Geometry"
#include <iostream>
#include "core/Time.h"
#include "math/Mathf.h"
#include "core/Log.h"

using namespace std;
using namespace Crawler;

void testSpeed(){

    Time::Start();

    Eigen::Vector3f vec_in(1.0f, 0.0f, 0.0f);

    // affine transformation
    Eigen::Affine3f affine = Eigen::Affine3f::Identity();

    Eigen::Vector3f vec_translation(2.0f, 2.0f, 1.0f);
    affine.translate(vec_translation);

    Eigen::Matrix4f mat_affine = affine.matrix();

    // transform vector
    cout << "testing speed ..." << endl;
    float t_start = Time::GetTime();
    int operations = 0;
    while(Time::GetTime() - t_start <= 5.0f){
        Eigen::Vector3f vec_out = affine * vec_in;
        operations++;
    }
    float t_duration = Time::GetTime() - t_start;
    float operations_per_second = (float)(operations) / t_duration;

    cout << "t_start: " << t_start << endl;
    cout << "t_duration: " << t_duration << endl;
    cout << "operations: " << operations << endl;
    cout << "operations_per_second: " << operations_per_second << endl;

}

int main(){

    // testSpeed();

    LogDebug("Test", iLog <<  (RIGHT_2_UNITY_3x3f * Eigen::Vector3f(1,0,0)).transpose());
    LogDebug("Test", iLog <<  (RIGHT_2_UNITY_3x3f * Eigen::Vector3f(0,1,0)).transpose());
    LogDebug("Test", iLog <<  (RIGHT_2_UNITY_3x3f * Eigen::Vector3f(0,0,1)).transpose());

    Eigen::Vector3f vec(1, 0, 0);
    vec = RIGHT_2_UNITY_3x3f * vec;
    vec[1] = 2;
    vec = UNITY_2_RIGHT_3x3f * vec;

    LogDebug("Test", iLog << vec.transpose());

    Eigen::Vector3f forward(1,1,1);
    Eigen::Vector3f upwards(0,0,1);
    Eigen::Matrix3f matrix = Mathf::LookRotationNew(forward, upwards);
    Eigen::Vector3f result = matrix * Eigen::Vector3f(1,0,0);

    LogDebug("Test", iLog << "matrix:\n" << matrix);
    LogDebug("Test", iLog << "forward: " << forward.transpose());
    LogDebug("Test", iLog << "upwards: " << upwards.transpose());
    LogDebug("Test", iLog << "result: " << result.transpose());




    // done
    return EXIT_SUCCESS;

}