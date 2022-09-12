#ifndef SPHERE_H
#define SPHERE_H

#include "vec3.h"
#include "hittable.h"

class sphere : public hittable {
    public:
        point3 center;
        double radius;
        shared_ptr<material> materialPtr;

        sphere(){}
        sphere(point3 cen, double r, shared_ptr<material> mP) : center(cen), radius(r), materialPtr(mP) {};

        virtual bool hit(const ray& r, double tMin, double tMax, hitRecord& rec) const override;

};



bool sphere::hit(const ray& r, double tMin, double tMax, hitRecord& rec) const{
    auto a = dotProduct(r.direction(), r.direction());
    auto h = dotProduct(r.direction(), r.origin() - center);
    auto c = dotProduct(r.origin() - center, r.origin() - center) - radius*radius;

    auto discriminant = h*h - a*c;
    if(discriminant < 0){
        return false;
    }
    auto sqrtdisc = sqrt(discriminant);

    //Find closest t val that is in the range
    auto root = (-h - sqrtdisc) / a;
    if(root < tMin || root > tMax){
        root = (-h + sqrtdisc) / a;
        if(root < tMin || root > tMax){
            return false;
        }
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outwardNormal = (rec.p - center) / radius;
    rec.setFaceNormal(r, outwardNormal);
    rec.materialPtr = materialPtr;

    return true;

}

#endif