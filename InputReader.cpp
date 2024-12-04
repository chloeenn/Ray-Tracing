
#include "InputReader.h"
#include <fstream>
#include <iostream>
#include "glm-master/glm/glm.hpp"

Scene parseInput(const std::string& fileName) {
    Scene scene;
    std::ifstream file(fileName);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string command;
        ss >> command;
        
        if (command == "NEAR") ss >> scene.nearPlane;
        else if (command == "LEFT") ss >> scene.left;
        else if (command == "RIGHT") ss >> scene.right;
        else if (command == "BOTTOM") ss >> scene.bottom;
        else if (command == "TOP") ss >> scene.top;
        else if (command == "RES") ss >> scene.resX >> scene.resY;
        else if (command == "SPHERE") {
            Sphere sphere;
            ss >> sphere.name
            >> sphere.position.x >> sphere.position.y >> sphere.position.z
            >> sphere.scale.x >> sphere.scale.y >> sphere.scale.z
            >> sphere.color.r >> sphere.color.g >> sphere.color.b
            >> sphere.Ka >> sphere.Kd >> sphere.Ks >> sphere.Kr >> sphere.n;
            scene.spheres.push_back(sphere);
        } else if (command == "LIGHT") {
            Light light;
            ss >> light.name >> light.position.x >> light.position.y >> light.position.z
            >> light.intensity.r >> light.intensity.g >> light.intensity.b;
            scene.lights.push_back(light);
        } else if (command == "BACK") {
            ss >> scene.backgroundColor.r >> scene.backgroundColor.g >> scene.backgroundColor.b;
        }
        else if (command == "AMBIENT") {
            ss >> scene.ambientIntensity.r >> scene.ambientIntensity.g >> scene.ambientIntensity.b;
        }
        else if (command == "OUTPUT") {
            ss >> scene.outputFile;
        }
    }
    return scene;
}
