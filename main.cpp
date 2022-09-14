#include "rtutilities.h"
#include "color.h"
#include "hittableList.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "triangle.h"
#include "ray.h"
#include "threadPool.h"

#include <iostream>
#include <thread>
#include <vector>


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

hittableList threadTest(){
    hittableList world;

    auto groundMaterial = make_shared<lambertian>(color(.5, .5, .5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, groundMaterial));

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 2), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(.4, .2, .1));
    world.add(make_shared<sphere>(point3(4, 1, -1), 1.0, material2));

    auto material3 = make_shared<metal>(color(.7, .6, .5), 0.0);
    world.add(make_shared<sphere>(point3(-3.5, 1, -1), 1.0, material3));
    

    return world;
}

hittableList triangleTest(){
    hittableList world;

    auto material = make_shared<metal>(color(.7, .6, .5), 0.0);
    world.add(make_shared<triangle>(point3(-.1, .1, -1), point3(0, 0, -1), point3(.1, .1, -1), material));
    world.add(make_shared<sphere>(point3(0, .1, 1), .1, material));
    return world;

}

hittableList superEpicFancy(){
    hittableList world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = randomDouble();
            point3 center(a + 0.9*randomDouble(), 0.2, b + 0.9*randomDouble());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = randomDouble(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
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
    point3 lookfrom(0, 0, 0);
    point3 lookat(0, 0, -1);
    vec3 vup(0, 1, 0);
    auto distToFocus = 10.0;
    auto aperture = 0;
    return camera(lookfrom, lookat, vup, 20, aspectRatio, aperture, distToFocus);
}




//Multithread code - only support image width of 1200, height of 800 b/c of array


void render(int imageWidth, int imageHeight, int maxDepth, camera& cam, hittableList& world, color* image){
    //unsigned int seed;
    //rand_r(&seed);
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



int main(){

    //Image
    const auto aspectRatio = 3.0/2.0;
    const int imageWidth = 1200;
    const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    int samplesPerPixel = 16;
    int maxDepth = 50;

    
    //World
    auto world = threadTest();

    //Camera
    camera cam = fancyCam();

    //Render

    color* image = new color[imageWidth * imageHeight];
    for(int j = imageHeight-1; j >= 0; j--){
        for(int i = 0; i < imageWidth; i++){
            image[j * imageWidth + i] = color(0, 0, 0);
        }
    }
    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

        //Multithreading

    
    threadPool pool;
    pool.start();
    for(unsigned int i = 0; i < samplesPerPixel; i++){
        std::function<void()> job = [&]() { render(imageWidth, imageHeight, maxDepth, cam, world, image); };
        pool.queueJob(job);
    }

    while(pool.busy()){
        std::cerr << "\rSamples remaining: " << pool.getQueueSize() + 8 << ' ' << std::fflush;
    }

    pool.stop();
    


    //render(imageWidth, imageHeight, samplesPerPixel, maxDepth, cam, world, image);
    for(int j = imageHeight-1; j >= 0; j--){
        for(int i = 0; i < imageWidth; i++){
            writeColor(std::cout, image[j * imageWidth + i], samplesPerPixel);
        }
    }
    delete[] image;
    std::cerr << "\nDone\n";

    


}