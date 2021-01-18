#ifndef Plane_h
#define Plane_h

#include "GLObject.h"

class Plane : public Object {
    public:

        Vec3f normal; // Vetor normal ao plano
        float distance; // Distância da origem

        Material material = Material(0.3, 1.0, 0.5f, 6);

        Plane(Vec3f n,
            float d, 
            Vec3f surfaceColor,
            Vec3f emissionColor = BLACK,
            Material material = Material(0.3, 1.0, 0.5f, 6),
            float transparency = 0)
            : normal(n.normalize()), distance(d), Object(surfaceColor, emissionColor, material, transparency) {}

        bool intersect(Ray& ray, float& t1, float& t2) const {
            if (normal.dot(ray.direction) >= 0) return false; // O produto escalar da interseção é menor que zero
            float o_d = abs(ray.origin.dot(normal) - distance);
            float cosa = -ray.direction.dot(normal);
            t1 = o_d / cosa;
            t2 = t1;
            return true;
        }

        Vec3f nhit(Vec3f& phit) const {
            return normal;
        }

};

#endif
