#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtutilities.h"

class material;

struct hitRecord{
    point3 p;
    vec3 normal;
    shared_ptr<material> materialPtr;
    double t;
    bool frontFace;

    inline void setFaceNormal(const ray& r, const vec3& outwardNormal){
        frontFace = dotProduct(r.direction(), outwardNormal) < 0;
        if(frontFace){
            normal = outwardNormal;
        } else {
            normal = -outwardNormal;
        }
    } 
};

class hittable{
    public:
        virtual bool hit(const ray& r, double tMin, double tMax, hitRecord& rec) const = 0;
};


#endif