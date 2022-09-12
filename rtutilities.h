#ifndef RTUTILITIES_H
#define RTUTILITIES_H

#include <assert.h>
#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

//Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

//Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

//Utility functions
inline double degToRad(double degrees){
    return (pi/180) * degrees;
}

inline double randomDouble(){
    return rand() / (RAND_MAX + 1.0);
}

inline double randomDouble(double min, double max){
    return min + randomDouble()*(max-min);
}

inline double clamp(double x, double min, double max){
    if(x < min){
        return min;
    } else if(x > max){
        return max;
    }
    return x;
}

//Common headers
#include "ray.h"
#include "vec3.h"
#include "hittable.h"

#endif