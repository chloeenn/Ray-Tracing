//light.h: structure of light source
#ifndef light_h
#define light_h
#include "glm-master/glm/glm.hpp"

struct Light
{
    std::string name;
    glm::vec4 position;
    glm::vec3 intensity;
};


#endif /* light_h */
