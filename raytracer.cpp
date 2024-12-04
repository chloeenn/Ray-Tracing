//Main file

#include <iostream>
#include <string>
#include "glm-master/glm/glm.hpp"
#include "ppm.h"
#include "invert.h"
#include "InputReader.h"
const int MAX_RECURSION_DEPTH = 3; //not spawn more than three reflection rays for each pixel

//isIntersect() determines whether or not the ray intersects the sphere
bool isIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const Sphere& sphere, double& hit) {
    glm::vec3 sphereCenter = glm::vec3(sphere.position);
    glm::vec3 oc = rayOrigin - sphereCenter;
    
    double a = glm::dot(rayDir, rayDir);
    double b = 2.0f * glm::dot(oc, rayDir);
    double c = glm::dot(oc, oc) - sphere.scale.x * sphere.scale.x;
    /*
     Use Quadratic Formula
     If discriminant < 0, No intersection
     Else (discriminant > 0: 2 intersections), return the nearest t
     */
    double discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0) return false;
    double t0 = (-b - sqrt(discriminant)) / (2.0f*a);
    double t1 = (-b + sqrt(discriminant)) / (2.0f*a);
    
    hit = (t0 > 0) ? t0 : t1;
    return hit > 0;
}

//generateRay() Ray from camPos to image Plane
glm::vec3 generateRay(const Scene& scene, int x, int y) {
    float u = scene.left + (scene.right - scene.left) * (x + 0.5f) / scene.resX;
    float v = scene.bottom + (scene.top - scene.bottom) * (y + 0.5f) / scene.resY;
    glm::vec3 rayDir = glm::normalize(glm::vec3(u, v, -scene.nearPlane));
    return rayDir;
}

//Check if the point is in shadow
bool isInShadow(const glm::vec3& hitPoint, const glm::vec3& lightDir, const Scene& scene) {
    for (const auto& sphere : scene.spheres) {
        double t;
        if (isIntersect(hitPoint + 0.001f * lightDir, lightDir, sphere, t)) {
            return true;
        }
    }
    return false;
}

//computeColor() Calculate surface normal and Lightning (ambient, diffuse, specular, reflective)
glm::vec3 trace(const Scene& scene, const glm::vec3& rayOrigin, const glm::vec3& rayDir, int depth);
glm::vec3 computeColor(const Scene& scene, const Sphere& sphere, const glm::vec3& hitPoint, const glm::vec3& normal, const glm::vec3& viewDir, int depth) {
    glm::vec3 color = sphere.Ka * scene.ambientIntensity * sphere.color; // Ambient
    
    for (const auto& light : scene.lights) {
        glm::vec3 lightDir = glm::normalize(glm::vec3(light.position) - hitPoint);
        
        // Check for shadows
        if (isInShadow(hitPoint, lightDir, scene)) {
            continue; // Skip this light if in shadow
        }
        
        // Diffuse
        float diff = glm::max(glm::dot(normal, lightDir), 0.0f);
        color += sphere.Kd * light.intensity * diff * sphere.color;
        
        // Specular
        glm::vec3 reflectDir = glm::reflect(-lightDir, normal);
        float spec = glm::pow(glm::max(glm::dot(viewDir, reflectDir), 0.0f), sphere.n);
        color += sphere.Ks * light.intensity * spec;
    }
    
    // Reflection
    if (depth < MAX_RECURSION_DEPTH && sphere.Kr > 0.0f) {
        glm::vec3 reflectDir = glm::reflect(-viewDir, normal);
        std::cout << "Reflection Direction: " << reflectDir.x << ", " << reflectDir.y << ", " << reflectDir.z << std::endl;
    }
    return glm::clamp(color, 0.0f, 1.0f);
}
//Ray into scene
glm::vec3 trace(const Scene& scene, const glm::vec3& rayOrigin, const glm::vec3& rayDir, int depth) {
    //DBL_MAX (largest double possible value) as Closest Intersection Distance because ray start at t=0
    double closestT = DBL_MAX;
    const Sphere* closestSphere = nullptr;
    
    for (const auto& sphere : scene.spheres) {
        double t;
        if (isIntersect(rayOrigin, rayDir, sphere, t) && t < closestT) {
            closestT = t;
            closestSphere = &sphere;
        }
    }
    
    if (closestSphere) {
        glm::vec3 hitPoint = rayOrigin + static_cast<float>(closestT) * rayDir;
        glm::vec3 normal = glm::normalize(hitPoint - glm::vec3(closestSphere->position));
        glm::vec3 viewDir = -rayDir;
        return computeColor(scene, *closestSphere, hitPoint, normal, viewDir, depth);
    }
    
    // Return backgroundColor if no intersection :(
    return scene.backgroundColor;
}

void render(const Scene& scene) {
    std::vector<glm::vec3> framebuffer(scene.resX * scene.resY, scene.backgroundColor);
    
    // Render each pixel
    for (int y = 0; y < scene.resY; ++y) {
        for (int x = 0; x < scene.resX; ++x) {
            glm::vec3 rayDir = generateRay(scene, x, y);
            glm::vec3 rayOrigin(0.0f, 0.0f, 0.0f); // Camera at origin
            framebuffer[y * scene.resX + x] = trace(scene, rayOrigin, rayDir, 0);
        }
    }
    
    unsigned char* pixelBuffer = new unsigned char[3 * scene.resX * scene.resY];
    for (int y = 0; y < scene.resY; ++y) {
        for (int x = 0; x < scene.resX; ++x) {
            glm::vec3 color = framebuffer[y * scene.resX + x];
            int idx = (y * scene.resX + x) * 3;
            pixelBuffer[idx] = static_cast<unsigned char>(glm::clamp(color.r, 0.0f, 1.0f) * 255.0f);
            pixelBuffer[idx + 1] = static_cast<unsigned char>(glm::clamp(color.g, 0.0f, 1.0f) * 255.0f);
            pixelBuffer[idx + 2] = static_cast<unsigned char>(glm::clamp(color.b, 0.0f, 1.0f) * 255.0f);
        }
    }
    
    save_imageP6(scene.resX, scene.resY, const_cast<char*>(scene.outputFile.c_str()), pixelBuffer);
    
    //dellocate
    delete[] pixelBuffer;
}

int main(const int argc, char** argv){
    Scene scene = parseInput(argv[1]);
//    std::cout << "Parsed Scene:\n";
//    std::cout << "Near Plane: " << scene.nearPlane << "\n";
//    std::cout << "Resolution: " << scene.resX << "x" << scene.resY << "\n";
//    std::cout << "Spheres:\n";
//    for (auto  sphere : scene.spheres) {
//        std::cout << "  " << sphere.name << ": ("
//        << sphere.position.x << ", " << sphere.position.y << ", " << sphere.position.z
//        << "), Scale: (" << sphere.scale.x << ", " << sphere.scale.y << ", " << sphere.scale.z << ")\n";
//    }
    render(scene);
    
    return 0;
}
