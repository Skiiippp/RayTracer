#include "rtutilities.h"
#include "color.h"
#include "hittableList.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "triangle.h"
#include "ray.h"

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

    /*
    for(int a = -11; a < 11; a++){
        for(int b = -11; b < 11; b++){
            auto chooseMat = randomDouble();
            point3 center(a + .9*randomDouble(), .2, b + .9*randomDouble());

            if((center - point3(4, .2, 0)).length() > .9){
                shared_ptr<material> sphereMaterial;

                if(chooseMat < .8){
                    //diffuse
                    auto albeto = color::random() * color::random();
                    sphereMaterial = make_shared<lambertian>(albeto);
                    world.add(make_shared<sphere>(center, .2, sphereMaterial));
                } else if(chooseMat < .95) {
                    //metal
                    auto albedo = color::random(.5, 1);
                    auto fuzz = randomDouble(0, 0.5);
                    sphereMaterial = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, .2, sphereMaterial));
                } else {
                    //glass
                    sphereMaterial = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, .2, sphereMaterial));
                }
            }
        }
    }*/

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

camera fancyCam(){
    double aspectRatio = 3.0/2.0;
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto distToFocus = 12.0;
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




void singleThread(int imageWidth, int imageHeight, int currentHeight, int samplesPerPixel, int maxDepth, camera& cam, hittableList& world, color image[], int threadNum){
    //one line at a time
    srand(threadNum);
    for(int i = 0; i < imageWidth; i++){
        color pixelColor(0, 0, 0);
        for(int s = 0; s < samplesPerPixel; s++){
            auto u = (i + randomDouble()) / (imageWidth - 1);
            auto v = (currentHeight + randomDouble()) / (imageHeight - 1);
            ray r = cam.getRay(u, v);
            pixelColor += rayColor(r, world, maxDepth);
            image[i] = pixelColor;
        }
    }
}

void multiThread(int imageWidth, int imageHeight, int currentHeight, int samplesPerPixel, int maxDepth, camera& cam, hittableList& world, color image[]){
    //4 threads by default

    color image0[1200]; //will need to be changed with a variable width
    std::thread t0(singleThread, imageWidth, imageHeight, currentHeight, samplesPerPixel, maxDepth, std::ref(cam), std::ref(world), image0, 0);


    color image1[1200];
    std::thread t1(singleThread, imageWidth, imageHeight, currentHeight, samplesPerPixel, maxDepth, std::ref(cam), std::ref(world), image1, 1);

    color image2[1200];
    std::thread t2(singleThread, imageWidth, imageHeight, currentHeight, samplesPerPixel, maxDepth, std::ref(cam), std::ref(world), image2, 2);

    color image3[1200];
    std::thread t3(singleThread, imageWidth, imageHeight, currentHeight, samplesPerPixel, maxDepth, std::ref(cam), std::ref(world), image3, 3);

    color image4[1200];
    std::thread t4(singleThread, imageWidth, imageHeight, currentHeight, samplesPerPixel, maxDepth, std::ref(cam), std::ref(world), image4, 4);

    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    for(int i = 0; i < imageWidth; i++){
        image[i] = image0[i] + image1[i] + image2[i] + image3[i] + image4[i];
    }
}



int main(){

    //Image
    const auto aspectRatio = 3.0/2.0;
    const int imageWidth = 1200;
    const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    const int samplesPerPixel = 10/5; //Div by 5 b/c 5 threads
    int maxDepth = 50;

    
    //World
    auto world = threadTest();

    //Camera
    camera cam = fancyCam();
    

    //Render
    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

    for(int j = imageHeight-1; j >= 0; j--){
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::fflush;
        
        /*
        for(int i = 0; i < imageWidth; i++){

            /*
            color pixelColor(0, 0, 0);
            for(int s = 0; s < samplesPerPixel; s++){
                auto u = (i + randomDouble()) / (imageWidth - 1);
                auto v = (j + randomDouble()) / (imageHeight - 1);
                ray r = cam.getRay(u, v);
                pixelColor += rayColor(r, world, maxDepth);
            }
            writeColor(std::cout, pixelColor, samplesPerPixel);
        }*/

            color image[imageWidth];
            multiThread(imageWidth, imageHeight, j, samplesPerPixel, maxDepth, cam, world, image);
            for(int i = 0; i < imageWidth; i++){
                writeColor(std::cout, image[i], samplesPerPixel*4);
            

        }

    }

    std::cerr << "\nDone\n";


}