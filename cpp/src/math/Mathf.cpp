#include "Mathf.h"
#include <cmath>

#define MATHF_PI 3.141592653589793238462643383279502884197
#define MATHF_SQRT2 1.41421356237309504880168872420969807856967187537

using namespace std;

const float PIf = MATHF_PI;
const float PI_INVf = 1.0 / MATHF_PI;
const float PI2f = 2.0*MATHF_PI;
const float PI2_INVf = 1.0 / (2.0*MATHF_PI);
const float SQRT2f = MATHF_SQRT2;
const float SQRT2_INVf = 1.0 / MATHF_SQRT2;
const float RAD_2_DEGf = 1.0 / MATHF_PI * 180.0;
const float DEG_2_RADf = 1.0 / 180.0 * MATHF_PI;

float Mathf::angle_to_symmetric(float angle){
    if(angle > +PIf){
        return angle - PI2f;
    } else if(angle < -PIf) {
        return angle + PI2f;
    }
    return angle;
}





