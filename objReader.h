#ifndef OBJRREADER_H
#define OBJREADER_H

#include "vec3.h"
#include "hittableList.h"
#include "triangle.h"
#include "material.h"
#include <memory>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdlib.h>

void triangulator(std::vector<point3>& vertices, hittableList& mesh){
    auto material2 = make_shared<metal>(color(.7, .6, .5), 0.0); //temporary for testing purposes
    int i = 0;
    int j = 1;
    int k = 2;
    while(k < vertices.size()){
        mesh.add(make_shared<triangle>(vertices[i], vertices[j], vertices[k], material2));
        j++;
        k++;
    }
}

point3 vertexFromLine(std::string& line){
    std::string::size_type sz;
    line.erase(0,2);
    long double x = std::stold(line, &sz);
    std::string secondLine = line.substr(sz);
    long double y = std::stold(secondLine, &sz);
    long double z = std::stold(secondLine.substr(sz));
    return point3(x, y, z);
}



std::vector<int> faceFromLine(std::string line){ 
    std::vector<int> vertices;
    line.erase(0,2);
    for(int i = 0; i < line.size(); i++){
        if(line[i] == '/'){
            while(line[i] != ' ' && i < line.size()){
                line[i] = ' ';
                i++;
            }
        }
    }
    int i = line.size() - 1;
    while(line[i-1] == ' '){
        i--;
    }
    line.erase(i, line.size() - i);
    std::string tempLine = line;
    tempLine.push_back('!');
    std::string::size_type sz;
  
    while(tempLine[0] != '!'){ 
        int index = std::stoi(tempLine, &sz);
        vertices.push_back(index);
        tempLine = tempLine.substr(sz);
    }

    

    return vertices;
}

hittableList objModel(std::string filename){
    std::ifstream stream(filename);
    std::vector<point3> vertices;
    hittableList mesh;

    int count = 0;
    while(!stream.eof()){
        std::string line;
        std::getline(stream, line);
        
        if(line[0] == 'v' && line[1] == ' '){
            vertices.push_back(vertexFromLine(line));
        } else if(line[0] == 'f' && count < 10){
            //count ++;
            std::vector<int> indeces = faceFromLine(line);  
            std::vector<point3> faceVertices;
            for(int index : indeces){
                faceVertices.push_back(vertices[index-1]);
            }
            triangulator(faceVertices, mesh);
        }
    }
    return mesh;
}






#endif