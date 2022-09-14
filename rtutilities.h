#ifndef RTUTILITIES_H
#define RTUTILITIES_H

#include <assert.h>
#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>
#include <random>

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
    thread_local static std::random_device rd;
    thread_local static std::mt19937 rng(rd());
    thread_local std::uniform_real_distribution<float> urd;
    return urd(rng, decltype(urd)::param_type{0, 1});
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