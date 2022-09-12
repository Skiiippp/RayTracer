#ifndef TRIANGLE_H
#define TRIANGLE_H


#include "rtutilities.h"
#include "vec3.h"
#include "hittable.h"
#include <iostream>

class triangle : public hittable {
    public:
        point3 A;   //vertices should be defined in counter clockwise order
        point3 B;
        point3 C;
        shared_ptr<material> materialPtr;

        triangle() {}
        triangle(point3 a, point3 b, point3 c, shared_ptr<material> mP) : A(a), B(b), C(c), materialPtr(mP) {} 

        virtual bool hit(const ray& r, double tMin, double tMax, hitRecord& rec) const override;

};

bool triangle::hit(const ray& r, double tMin, double tMax, hitRecord& rec) const {

    vec3 n = unitVector(crossProduct(B-A, C-A));
    double d = dotProduct(n, A);
    vec3 D = r.direction();
    vec3 P = r.origin();
    double someValue = dotProduct(n, D);
    if(someValue == 0){
        return false;
    }
    double t = (d-dotProduct(n, P)) / someValue;

    if(t < tMin || t > tMax){
        return false;
    }


    point3 Q = r.at(t);
    if(dotProduct(crossProduct(B-A, Q-A), n) >= 0 && dotProduct(crossProduct(A-C, Q-C), n) >= 0 && dotProduct(crossProduct(C-B, Q-B), n) >= 0){
        
        rec.t = t;
        rec.p = Q;
        rec.setFaceNormal(r, n);
        rec.materialPtr= materialPtr;

        return true;
    }
    return false;


}



#endif 