#ifndef Sphere_h
#define Sphere_h

#include "GLObject.h"


class Sphere : public Object {

    public:

        Vec3f center;
        float radius;
        float radius2;

        Sphere(Vec3f c, 
            float r, 
            Vec3f sc,
            float trans, 
            Vec3f ec = BLACK,
            Material material = Material(0.3, 1.0, 0.9, 50)) 
            : center(c), radius(r), radius2(r* r), Object(sc, trans, ec, material) {}

        bool intersect(Ray& ray, float& t1, float& t2) const {
            Vec3f distance = center - ray.origin;
            float distanceProjection = distance.dot(ray.direction);
            if (distanceProjection < 0) return false;//não se cruzam
            float distanceLength2 = distance.length2();
            float rdistance2 = distanceLength2 - distanceProjection * distanceProjection;
            if (rdistance2 > radius2) return false;//não se cruzam
            float interception = sqrt(radius2 - rdistance2);//Metade da interceptação
            t1 = distanceProjection - interception;
            t2 = distanceProjection + interception;
            return true;
        }

        // Normalizar
        Vec3f nhit(Vec3f& phit) const {
            return (phit - center).normalize();
        }



        Vec3f getTextureColor(Vec3f& phit) const {
            return WHITE;
        }
};


#endif


