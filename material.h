#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtutilities.h"


struct hitRecord;

class material{
    public:
        virtual bool scatter(const ray& rayIn, const hitRecord& rec, color& attenuation, ray& scattered) const = 0;
};

class lambertian : public material{
    public:
        color albedo;

        lambertian(const color& a) : albedo(a) {}

        virtual bool scatter(const ray& rayIn, const hitRecord& rec, color& attenuation, ray& scattered) const override {
            auto scatterDirection = rec.normal + randomUnitVector();

            if(scatterDirection.nearZero()){
                scatterDirection = rec.normal;
            }

            scattered = ray(rec.p, scatterDirection);
            attenuation = albedo;
            return true;
        }
};

class metal : public material{
    public:
        color albeto;
        double fuzz;

        metal(const color& a, double f) : albeto(a), fuzz(f<1 ? f : 1) {}

        virtual bool scatter(const ray& rayIn, const hitRecord& rec, color& attenuation, ray& scattered) const override {
            vec3 scatterDirection = reflect(unitVector(rayIn.direction()), rec.normal);
            scattered = ray(rec.p, scatterDirection + fuzz*randomInUnitSphere());
            attenuation = albeto;
            return(dotProduct(scatterDirection, rec.normal) > 0);
        }


};

class dielectric : public material{
    public:
        double ir; //Index of refraction

        dielectric(double indexOfRefrac) : ir(indexOfRefrac) {}

        virtual bool scatter(const ray& rayIn, const hitRecord& rec, color& attenuation, ray& scattered) const override {
            attenuation = color(1.0, 1.0, 1.0);
            double refractionRatio = rec.frontFace ? (1.0/ir) : ir;

            vec3 unitDirection = unitVector(rayIn.direction());
            double cosTheta = fmin(dotProduct(-unitDirection, rec.normal), 1.0);
            double sinTheta = sqrt(1.0 - cosTheta*cosTheta);

            bool cannotRefract = refractionRatio * sinTheta > 1;
            vec3 direction;
            if(cannotRefract || reflectance(cosTheta, refractionRatio) > randomDouble()){
                direction = reflect(unitDirection, rec.normal);
            } else{
                direction = refract(unitDirection, rec.normal, refractionRatio);
            }

            scattered = ray(rec.p, direction);
            return true;
        }

    private:
        static double reflectance(double cosine, double refIdx){ //Schlick's approx
            auto r0 = (1-refIdx) / (1+refIdx);
            r0 *= r0;
            return r0 + (1-r0)*pow((1-cosine), 5);
        }

};

#endif