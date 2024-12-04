#ifndef InputReader_h
#define InputReader_h

#include <stdio.h>
#include <fstream>
#include <sstream>
#include "sphere.h"
#include "light.h"
#include <vector>

struct Scene {
    float nearPlane, left, right, bottom, top; //Near Plane, Frustrum
    int resX, resY; //Resolution of image: X_column, Y_row
    std::vector<Sphere> spheres; 
    std::vector<Light> lights;
    glm::vec3 backgroundColor;
    glm::vec3 ambientIntensity;
    std::string outputFile;
};

Scene parseInput(const std::string& fileName);
#endif /* InputReader_h */
