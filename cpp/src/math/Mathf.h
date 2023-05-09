#pragma once

#include "Eigen/Fix"
#include "Eigen/Geometry"

namespace Crawler {

extern const float PIf;
extern const float PI_INVf;
extern const float PI2f;
extern const float PI2_INVf;
extern const float SQRT2f;
extern const float SQRT2_INVf;
extern const float RAD_2_DEGf;
extern const float DEG_2_RADf;

extern const Eigen::Matrix3f UNITY_2_RIGHT_3x3f;
extern const Eigen::Matrix3f RIGHT_2_UNITY_3x3f;

class Mathf {
public:

    static float AngleToSymmetric(float angle);

    static Eigen::Quaternionf LookRotationUnity(const Eigen::Vector3f& forward, const Eigen::Vector3f& upwards);

    static Eigen::Quaternionf LookRotation(const Eigen::Vector3f& forward, const Eigen::Vector3f& upwards);

    static Eigen::Matrix3f LookRotationNew(const Eigen::Vector3f& forward, const Eigen::Vector3f& upwards);

    static void Vector3ToString(const Eigen::Vector3f& v, const char* format, char* out);

};

}




