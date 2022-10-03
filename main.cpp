#include "rtutilities.h"
#include "color.h"
#include "hittableList.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "triangle.h"
#include "ray.h"
#include "threadPool.h"
#include "objReader.h"

#include <iostream>
#include <thread>
#include <vector>
#include <string>


color rayColor(const ray& r, hittable& world, int depth){

        if(depth <= 0){
            return color(0, 0, 0); //Light has "stopped emitting energy" I guess
        }

        hitRecord record;
        if(world.hit(r, 0.001, infinity, record)){
            ray scattered;
            color attenuation;
            if(record.materialPtr->scatter(r, record, attenuation, scattered)){
                return attenuation * rayColor(scattered, world, depth -1);
            }
            return color(0, 0, 0);
        }

        vec3 unitDirection = unitVector(r.direction());
        auto t = .5 * (unitDirection.y()+1.0);
        return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(.5, .7, 1.0); //linear interpolation formula 

}



camera fancyCam(){
    double aspectRatio = 3.0/2.0;
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto distToFocus = 10.0;
    auto aperture = .1;
    return camera(lookfrom, lookat, vup, 20, aspectRatio, aperture, distToFocus);
    
}

camera testCam(){
    double aspectRatio = 3.0/2.0;
    point3 lookfrom(-10, 2, 15);
    point3 lookat(0, -.6, 0);
    vec3 vup(0, 1, 0);
    auto distToFocus = 10.0;
    auto aperture = 0;
    return camera(lookfrom, lookat, vup, 20, aspectRatio, aperture, distToFocus);
}

hittableList testWorld(){
    std::string fileName = "suzanne.obj";
    auto world = objModel(fileName);
    auto mat = make_shared<lambertian>(color(.5, .5, .5));
    world.add(make_shared<triangle>(point3(0, 0, -100), point3(500, 0, 500), point3(-500, 0, 500),  mat));
    return world;
}

void render(int imageWidth, int imageHeight, int maxDepth, camera& cam, hittableList& world, color* image){
    for(int j = imageHeight-1; j >= 0; j--){
        for(int i = 0; i < imageWidth; i++){   
            auto u = (i + randomDouble()) / (imageWidth - 1);
            auto v = (j + randomDouble()) / (imageHeight - 1);
            ray r = cam.getRay(u, v);
            color pixelColor = rayColor(r, world, maxDepth);
            image[j * imageWidth + i] += pixelColor;
        } 
    }
}

void lineRender(int imageWidth, int imageHeight, int maxDepth, camera& cam, hittableList& world, color* image, int currentHeight){
    std::cerr << currentHeight << "\n";    //Always 0?
    for(int i = 0; i < imageWidth; i++){
            auto u = (i + randomDouble()) / (imageWidth - 1);
            auto v = (currentHeight + randomDouble()) / (imageHeight - 1);
            ray r = cam.getRay(u, v);
            color pixelColor = rayColor(r, world, maxDepth);

            image[currentHeight * imageWidth + i] += pixelColor; 
            
        } 
        
}

int main(){

    //Image
    const auto aspectRatio = 3.0/2.0;
    const int imageWidth = 100;
    const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    int samplesPerPixel = 1;
    int maxDepth = 50;

    
    //World
    auto world = testWorld();

    //Camera
    camera cam = testCam();

    //Render

    color* image = new color[imageWidth * imageHeight];
    for(int j = imageHeight-1; j >= 0; j--){
        for(int i = 0; i < imageWidth; i++){
            image[j * imageWidth + i] = color(0, 0, 0);
        }
    }
    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

        //Multithreading

    /*
    threadPool pool;
    pool.start();

    
    for(unsigned int i = 0; i < samplesPerPixel; i++){
        std::function<void()> job = [&]() { render(imageWidth, imageHeight, maxDepth, cam, world, image); };
        pool.queueJob(job);
    }
    
    while(pool.busy()){
        std::cerr << "\rLines remaining in queue: " << pool.getQueueSize() << ' ' << std::fflush;
    }
    pool.stop();
    */

    //Sequential test -- Errors were not based on multithreading

    std::vector<std::function<void()>> jobList;

    for(int i = imageHeight - 1; i >= 0; i--){
        std::function<void()> job = [&]() { lineRender(imageWidth, imageHeight, maxDepth, cam, world, image, i); };
        jobList.push_back(job);
    }


    int count = 0;
   
    for(std::function<void()> job : jobList){  
        job();
        count++;
    }

    
    

    for(int j = imageHeight-1; j >= 0; j--){
        for(int i = 0; i < imageWidth; i++){
            writeColor(std::cout, image[j * imageWidth + i], samplesPerPixel);
        }
    }
    delete[] image;
    std::cerr << "\nDone\n";

    


}