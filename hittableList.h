#ifndef HITTABLELIST_H
#define HITTABLELIST_H

#include "hittable.h"
#include "rtutilities.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittableList : public hittable {
    public:
        std::vector<shared_ptr<hittable>> objects;
        
        hittableList() {}
        hittableList(shared_ptr<hittable> object){
            add(object);
        }

        void add(shared_ptr<hittable> object){
            objects.push_back(object);
        }
        void clear() {
            objects.clear();
        }

        virtual bool hit (const ray& r, double tMin, double tMax, hitRecord& rec) const override;
};

bool hittableList::hit(const ray&r, double tMin, double tMax, hitRecord& rec) const{
    bool check = false;
    for(const auto& object : objects){
        if(object->hit(r, tMin, tMax, rec)){
            check = true;
            tMax = rec.t;
        }
    }
    return check;
}


#endif
