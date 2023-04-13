#include "Eigen/Fix"
#include "Eigen/Geometry"
#include <iostream>
#include "core/Time.h"

using namespace std;
using namespace Crawler;

int main(){

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

    // done
    return EXIT_SUCCESS;

}