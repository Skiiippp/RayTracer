#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>


using std::sqrt;

class vec3 {
    public:

        double e[3];

        vec3() : e{0, 0 ,0} {}
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

        double x() const {
            return e[0];
        }
        double y() const {
            return e[1];
        }
        double z() const {
            return e[2];
        }

        vec3 operator-() const {
            return vec3(-e[0], -e[1], -e[2]);
        }
        double operator[](int i) const {
            return e[i];
        }
        double& operator[](int i)  {
            return e[i];
        }

        vec3& operator+=(const vec3 &v){
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3& operator*=(const double t){
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(const double t){
            *this *= 1/t;
            return *this;
        }

        double lengthSquared() const {
            return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
        }

        double length() const{
            return sqrt(lengthSquared());
        }

        inline static vec3 random(){
            return vec3(randomDouble(), randomDouble(), randomDouble());
        }

        inline static vec3 random(double min, double max){
            return vec3(randomDouble(min, max), randomDouble(min, max), randomDouble(min, max));
        }

        bool nearZero() const{
            const auto smallBoi = 1e-8;
            return (fabs(e[0]) < smallBoi && fabs(e[1]) < smallBoi && fabs(e[2]) < smallBoi);
        }


        

};

//Type aliases
using point3 = vec3; //3D point
using color = vec3; //RGB color




//Utility functions

inline 
std::ostream& operator<<(std::ostream &out, const vec3 &v){
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v){
    return vec3(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v){
    return vec3(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v){
    return vec3(u[0] * v[0], u[1] * v[1], u[2] * v[2]);
}

inline vec3 operator*(double t, const vec3 &v){
    return vec3(t * v[0], t * v[1], t * v[2]);
}

inline vec3 operator*(const vec3 &v, double t){
    return t * v;
}

inline vec3 operator/(vec3 v, double t){
    return (1/t) * v;
}

inline double dotProduct(const vec3 &u, const vec3 &v){
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

inline vec3 crossProduct(const vec3 &u, const vec3 &v){
    return vec3(u[1]*v[2] - u[2]*v[1], -(u[0]*v[2] - u[2]*v[0]), u[0]*v[1] - u[1]*v[0]);
}

inline vec3 unitVector(vec3 v){
    return v / (v.length());
}

vec3 randomInUnitSphere(){
    while(true){
        auto p = vec3::random(-1, 1);
        if(p.lengthSquared() <= 1){
            return p;
        }
    }
}

vec3 randomInUnitDisc(){
    while(true){
        auto p = vec3(randomDouble(-1, 1), randomDouble(-1, 1), 0);
        if(p.lengthSquared() < 1){
            return p;
        }
    }
}

vec3 randomUnitVector(){
    return unitVector(randomInUnitSphere());
}


vec3 reflect(const vec3& v, const vec3& normal){
    double b = -dotProduct(v, normal);
    return 2*b*normal + v;
}

vec3 refract(const vec3& uv, const vec3& n, double etaOverEtaPrime){
    auto cosTheta = fmin(dotProduct(-uv, n), 1.0);
    vec3 rOutPerp = etaOverEtaPrime * (uv + cosTheta*n);
    vec3 rOutPara = -sqrt(fabs(1.0 - rOutPerp.lengthSquared())) * n;
    return rOutPara + rOutPerp;
}



#endif