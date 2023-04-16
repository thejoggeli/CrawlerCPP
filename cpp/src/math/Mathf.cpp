#include "Mathf.h"
#include <cmath>

#define MATHF_PI 3.141592653589793238462643383279502884197
#define MATHF_SQRT2 1.41421356237309504880168872420969807856967187537

using namespace std;

namespace Crawler {

const float PIf = MATHF_PI;
const float PI_INVf = 1.0 / MATHF_PI;
const float PI2f = 2.0*MATHF_PI;
const float PI2_INVf = 1.0 / (2.0*MATHF_PI);
const float SQRT2f = MATHF_SQRT2;
const float SQRT2_INVf = 1.0 / MATHF_SQRT2;
const float RAD_2_DEGf = 1.0 / MATHF_PI * 180.0;
const float DEG_2_RADf = 1.0 / 180.0 * MATHF_PI;

const Eigen::Matrix3f UNITY_2_RIGHT_3x3f = [] { 
    Eigen::Matrix3f matrix;
    matrix.row(0) <<  0.0f, 0.0f, 1.0f; 
    matrix.row(1) << -1.0f, 0.0f, 0.0f;
    matrix.row(2) <<  0.0f, 1.0f, 0.0f;
    return matrix;
}();

const Eigen::Matrix3f RIGHT_2_UNITY_3x3f = UNITY_2_RIGHT_3x3f.inverse();

float Mathf::AngleToSymmetric(float angle){
    if(angle > +PIf){
        return angle - PI2f;
    } else if(angle < -PIf) {
        return angle + PI2f;
    }
    return angle;
}

// https://gamedev.stackexchange.com/questions/198263/lookrotation-function-for-z-up-right-handed-system
Eigen::Quaternionf Mathf::LookRotation(const Eigen::Vector3f& forward, const Eigen::Vector3f& upwards){
    
    Eigen::Vector3f forwardUnity = RIGHT_2_UNITY_3x3f * forward;
    Eigen::Vector3f upwardsUnity = RIGHT_2_UNITY_3x3f * upwards;
    // Eigen::Vector3f forwardUnity = UNITY_2_RIGHT_3x3f * forward;
    // Eigen::Vector3f upwardsUnity = UNITY_2_RIGHT_3x3f * upwards;

    Eigen::Quaternionf quaternion = LookRotationUnity(forwardUnity, upwardsUnity);

    return Eigen::Quaternionf(-quaternion.y(), quaternion.z(), quaternion.x(), quaternion.w());
    // return Eigen::Quaternionf(-quaternion.z(), quaternion.x(), -quaternion.y(), quaternion.w());

}

// https://answers.unity.com/questions/467614/what-is-the-source-code-of-quaternionlookrotation.html
Eigen::Quaternionf Mathf::LookRotationUnity(const Eigen::Vector3f& forward, const Eigen::Vector3f& upwards){
    
    Eigen::Vector3f vector = forward.normalized();
    Eigen::Vector3f vector2 = upwards.cross(vector);
    Eigen::Vector3f vector3 = vector.cross(vector2);
    float m00 = vector2.x();
    float m01 = vector2.y();
    float m02 = vector2.z();
    float m10 = vector3.x();
    float m11 = vector3.y();
    float m12 = vector3.z();
    float m20 = vector.x();
    float m21 = vector.y();
    float m22 = vector.z();
 
    float num8 = (m00 + m11) + m22;
    Eigen::Quaternionf quaternion;

    if (num8 > 0.0f){
        float num = sqrt(num8 + 1.0f);
        quaternion.w() = num * 0.5f;
        num = 0.5f / num;
        quaternion.x() = (m12 - m21) * num;
        quaternion.y() = (m20 - m02) * num;
        quaternion.z() = (m01 - m10) * num;
        return quaternion;
    }
    if ((m00 >= m11) && (m00 >= m22)){
        float num7 = sqrt(((1.0f + m00) - m11) - m22);
        float num4 = 0.5f / num7;
        quaternion.x() = 0.5f * num7;
        quaternion.y() = (m01 + m10) * num4;
        quaternion.z() = (m02 + m20) * num4;
        quaternion.w() = (m12 - m21) * num4;
        return quaternion;
    }
    if (m11 > m22){
        float num6 = sqrt(((1.0f + m11) - m00) - m22);
        float num3 = 0.5f / num6;
        quaternion.x() = (m10+ m01) * num3;
        quaternion.y() = 0.5f * num6;
        quaternion.z() = (m21 + m12) * num3;
        quaternion.w() = (m20 - m02) * num3;
        return quaternion; 
    }
    float num5 = sqrt(((1.0f + m22) - m00) - m11);
    float num2 = 0.5f / num5;
    quaternion.x() = (m20 + m02) * num2;
    quaternion.y() = (m21 + m12) * num2;
    quaternion.z() = 0.5f * num5;
    quaternion.w() = (m01 - m10) * num2;
    return quaternion;

}

// https://github.com/OpenGP/htrack/blob/master/util/eigen_opengl_helpers.h
// https://stackoverflow.com/questions/19740463/lookat-function-im-going-crazy
Eigen::Matrix3f Mathf::LookRotationNew(const Eigen::Vector3f& forward, const Eigen::Vector3f& upwards){
    Eigen::Vector3f f = forward.normalized();
    Eigen::Vector3f u = upwards.normalized();
    Eigen::Vector3f s = f.cross(u).normalized();
    u = s.cross(f);
    Eigen::Matrix3f mat;
    mat(0,0) = f.x();
    mat(1,0) = f.y();
    mat(2,0) = f.z();
    mat(0,1) = s.x();
    mat(1,1) = s.y();
    mat(2,1) = s.z();
    mat(0,2) = u.x();
    mat(1,2) = u.y();
    mat(2,2) = u.z();
    return mat;
}


}


