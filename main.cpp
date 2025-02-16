#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <sstream>

#include "GLVector.h"
#include "GLObject.h"

#include "Cube.h"
#include "Sphere.h"
#include "Plane.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define MAX_DEPTH 4

#define AmbientStrength 0.3f
#define AmbientLightColor WHITE

Vec3f cameraPos(25, 0, 10);
Vec3f lookAt(0, 0, 0);
Vec3f up(0, 0, 1);


float mix(const float &a, const float &b, const float &mix) {
    return b * mix + a * (1 - mix);
}

// BRDF
Vec3f trace(Ray &ray, std::vector<Object *> objects, std::vector<Sphere *> lightings, int depth){
    
    float t_min = INFINITY;
    Object *object = NULL;
    Vec3f color = BLACK;
    float bias = 1e-4;
    
    std::size_t obj_size = objects.size();
    std::size_t light_size = lightings.size();

    for(int i=0;i<obj_size;i++) {
        float t1=INFINITY, t2 =INFINITY;
        if(objects[i]->intersect(ray, t1, t2)){
            if(t1<0) t1 = t2;
            if(t1<t_min) {
                t_min = t1;
                object = objects[i];
            }
        }
    }

    for(int i=0;i<light_size;i++){
        float t1=INFINITY, t2=INFINITY;
        if(lightings[i]->intersect(ray, t1, t2)){
            if(t1<0) t1=t2;
            if(t1<t_min){
                t_min = t1;
                object = lightings[i];
            }
        }
    }
    
    float t = INFINITY;
    if (t < t_min) {
        t_min = t;
        object = NULL;
    }
    
    // Cruzamo-nos com um objeto
    if(object != NULL) {
        
        // Ponto de interseção
        Vec3f phit = ray.getPoint(t_min);
        // Normal a esse ponto
        Vec3f nhit = object->nhit(phit); 
        
        // Efeito Fresnel
        float IdotN = ray.direction.dot(nhit);
        float facingratio = std::max(float(0), - IdotN);
        float fresneleffect = mix(pow(1 - facingratio, 3), 1, 0.3);
        
        // v dot n
        float rayProjection = abs(ray.direction.dot(nhit));
        // Direção
        Vec3f reflDirection = nhit * 2 * rayProjection + ray.direction;
        // Raio de reflexão
        Ray reflectRay = Ray(phit, reflDirection);
        
        color += object->surfaceColor * object->material.Ka * AmbientLightColor * AmbientStrength;
        
        color += object->emissionColor;
        
        if (object->material.Ks > 0 && depth < MAX_DEPTH) {

            /**
                A luz especular é calculada calculando o _raio de reflexão_, refletindo o vetor da luz sobre a normal no ponto de interseção.
                O raio de visão é comparado com o raio de reflexão para determinar quanta iluminação especular deve contribuir.
                Quanto mais paralelos são os vetores, mais iluminação especular haverá.
            
            **/
            
            // Reflexão especular
            Vec3f reflColor = trace(reflectRay, objects, lightings, depth+1);
            
            // Comparamos o raio da reflexão com o raio da visão
            Vec3f h = (reflDirection - ray.direction).normalize();
            
            // A reflexão especular não tem nada a ver com a cor da superfície
            color += (reflColor * fresneleffect *  std::max(0.0, pow(h.dot(nhit), object->material.shininess))) * object->material.Ks;
        }
        
        if (object->material.Kd > 0) {

            double shadow = 1.0;

            for(int i=0; i < light_size; i++) {

                /**
                Sombras -> Para determinar o quanto uma fonte de luz deve contrinuir para a iluminação em um ponto de interseção, disparamos um ray desde a interseção até a fonte de luz

                Se existir alguma interseção antes da luz, então esse ponto está na sombra de uma luz
                
                **/

                // Distância até a luz
                float D = (lightings[i]->center - phit).length() - lightings[i]->radius;
                // A intensidade da luz diminui com a distância
                float atten = 1 / (1 + 0.03 * D + 0.001 * D * D);
                // vetor que aponta para a luz
                Vec3f s = (lightings[i]->center - phit).normalize();
                // origem
                Vec3f origin = phit + (nhit * bias);
                // ray
                Ray lightRay(origin, s);
                // transmissão
                Vec3f transmission(1.0f);

                // Objeto a fazer sombra
                Object *shadowCast = NULL;
                float tnear = INFINITY;
                
                // Procuramos nos objetos as interseções
                for(int j =0; j<obj_size; j++) {
                    float t1=INFINITY, t2=INFINITY;
                    if(objects[j]->intersect(lightRay, t1, t2)){
                        if(t1<tnear) {
                            tnear = t1;
                            shadowCast = objects[j];
                        }
                    }
                }
                for(int j =0;j<light_size;j++){
                    float t1=INFINITY, t2=INFINITY;
                    if(i!=j && lightings[j]->intersect(lightRay, t1, t2)){
                        if(t1<tnear){
                            tnear = t1;
                            shadowCast = lightings[j];
                        }
                    }
                }
                
                // Existe um objeto no meio ?
                if(shadowCast) { 
                    shadow = std::max(0.0, shadow - (1.0 - shadowCast->transparency));
                    transmission = transmission * shadowCast->surfaceColor * shadow;
                }

                color += object->surfaceColor * lightings[i]->emissionColor * transmission * atten * std::max(0.0f, s.dot(nhit)) * object->material.Kd;
            }
        }
        
    }
    
    return color;
}

void render(const std::vector<Object *> objects, const std::vector<Sphere *>lightings,Camera &camera, GLFWwindow *window){
    
    
    unsigned char* pix = new unsigned char[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
    
    Vec3f *image = new Vec3f[WINDOW_WIDTH * WINDOW_HEIGHT], *pixel = image;
    
    for (unsigned y = 0; y < WINDOW_HEIGHT; ++y) {
        for (unsigned x = 0; x < WINDOW_WIDTH; ++x, ++pixel) {
            Ray ray = camera.generateRay(x, y, WINDOW_WIDTH, WINDOW_HEIGHT);
            int depth = 0;
            Vec3f color = trace(ray, objects, lightings, depth);
            *pixel = color;
        }
    }

    pixel = image;
    for (unsigned i = 0; i < WINDOW_HEIGHT; i++)
        for (unsigned j = 0; j < WINDOW_WIDTH; j++, pixel++) {
            pix[3 * (i*WINDOW_WIDTH + j)] = std::min(pixel->x, float(1)) * 255;
            pix[3 * (i*WINDOW_WIDTH + j) + 1] = std::min(pixel->y, float(1)) * 255;
            pix[3 * (i*WINDOW_WIDTH + j) + 2] = std::min(pixel->z, float(1)) * 255;
        }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pix);
        glfwSwapBuffers(window);
    }
    delete[] pix;
    glfwTerminate();
}

int main(int argc, const char * argv[]) {
    
    std::vector<Object *> objects;
    std::vector<Sphere *> lightings;
    
    lightings.push_back(new Sphere(Vec3f(20, 12, 20), 5, WHITE, WHITE, Material(0.3,1,0)));
    
    Cube * cube1 = new Cube(Vec3f(-6,-4,0), Vec3f(8, 8, 8), Vec3f(1.0f,1.0f,1.0f), Vec3f(0,0,0.2), Material(0.22, 0, 0.58, 0));
    objects.push_back(cube1);

    // temos transparencia na sombra e reflexao
    Sphere* sphere1 = new Sphere(Vec3f(7, -3, 3),2,Vec3f(1.0f,1.0f,1.0f), Vec3f(0.5,0,0), Material(0.22,0,1,1));

    objects.push_back(sphere1);
    objects.push_back(new Sphere(Vec3f(7, 2,1.5),1.5,Vec3f(0.5f,1.0f,1.0f),0, Material(0.22, 1, 0, 1)));

    objects.push_back(new Plane(Vec3f(0.0f,0.0f,1.0f), 0.0f, Vec3f(0.8f,0.8f,0.8f), BLACK, Material(0.3, 1, 0.1, 10)));
    
    Camera camera(cameraPos, lookAt, up, 60);
    
    glewExperimental=true;
    if( !glfwInit())
    {
        return -1;
    }

//    glfwWindowHint(GLFW_SAMPLES, 4);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Ray Tracing", nullptr, nullptr);

    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    glewExperimental=true;
    if(glewInit()!=GLEW_OK)
    {
        return -1;
    }
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    
    render(objects, lightings, camera, window);
    
    return 0;
    
}
