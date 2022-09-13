#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>

void writeColor(std::ostream &out, color pixelColor, int samplesPerPixel){

    auto r = pixelColor.x();
    auto g = pixelColor.y();
    auto b = pixelColor.z();

    auto scale = 1.0 / samplesPerPixel;
    //Gamma = 2
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    

    //Write translated (0-255) value of each color component
    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' ' 
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' ' 
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

color writeColor(color pixelColor, int samplesPerPixel){
    auto r = pixelColor.x();
    auto g = pixelColor.y();
    auto b = pixelColor.z();

    auto scale = 1.0 / samplesPerPixel;
    //Gamma = 2
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    
    
    //Write translated (0-255) value of each color component
    return color(static_cast<int>(256 * clamp(r, 0.0, 0.999)), static_cast<int>(256 * clamp(g, 0.0, 0.999)), static_cast<int>(256 * clamp(b, 0.0, 0.999)));  
}

#endif