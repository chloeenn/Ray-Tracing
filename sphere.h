//sphere.h: sphere structure
#ifndef sphere_h
#define sphere_h
#include <string.h>
#include "glm-master/glm/glm.hpp"
struct Sphere
{
    std::string name;
    glm::vec4 position;
    glm::vec3 scale;
    glm::vec3 color; //R,G,B
    float Ka, Kd, Ks, Kr; //ambient, diffuse, specular, reflective
    int n; //Specular exponent n
    glm::mat4 transformationMatrix;
    glm::mat4 inverseMatrix;
};

#endif /* sphere_h */
