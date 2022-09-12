#ifndef CAMERA_H
#define CAMERA_H

#include "rtutilities.h"

class camera{
    private:
        point3 origin;
        point3 lowerLeftCorner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        double lensRadius;
        
    public:
        camera(point3 lookfrom, point3 lookat, vec3 vup, double vfov, double aspectRatio, double aperture, double focusDist) {       
            //vertical fov in degrees

            auto theta = degToRad(vfov);
            auto h = tan(theta/2);
            auto viewportHeight = 2.0 * h;
            auto viewportWidth = aspectRatio * viewportHeight;

            w = unitVector(lookfrom - lookat);
            u = unitVector(crossProduct(vup, w));
            v = crossProduct(w, u);

            origin = lookfrom;
            horizontal = focusDist * viewportWidth * u;
            vertical = focusDist * viewportHeight * v; 
            lowerLeftCorner = origin - horizontal/2 - vertical/2 - focusDist*w;

            lensRadius = aperture/2;
        }

        ray getRay(double s, double t) const{
            vec3 rd = lensRadius * randomInUnitDisc();
            vec3 offset = u * rd.x() + v * rd.y();

            return ray(origin + offset, lowerLeftCorner + s*horizontal + t*vertical - (origin+offset));
        }
    
};

#endif