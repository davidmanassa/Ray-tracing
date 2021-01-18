#ifndef Cube_h
#define Cube_h

#include "GLObject.h"

#define smallEnough 1e-4

class Cube : public Object {

    public:

        Vec3f position;

        Vec3f length;

        Cube(Vec3f v,
            Vec3f length,
            Vec3f surfaceColor,
            Vec3f emissionColor = BLACK,
            Material material = Material(0.3, 1, 0.9, 6),
            float transparency = 0)
            : position(v), length(length), Object(surfaceColor, emissionColor, material, transparency) {}

        bool intersect(Ray& ray, float& t1, float& t2) const {
            // cruzamento externo

            Vec3f nTop(0.0f, 0.0f, 1.0f); //O vetor normal das seis faces do cubo
            Vec3f nBottom(0.0f, 0.0f, -1.0f);
            Vec3f nLeft(0.0f, -1.0f, 0.0f);
            Vec3f nRight(0.0f, 1.0f, 0.0f);
            Vec3f nFront(1.0f, 0.0f, 0.0f);
            Vec3f nBack(-1.0f, 0.0f, 0.0f);

            std::vector<Vec3f> normals;
            normals.push_back(nTop);
            normals.push_back(nBottom);
            normals.push_back(nLeft);
            normals.push_back(nRight);
            normals.push_back(nFront);
            normals.push_back(nBack);

            for (int i = 0; i < 6; i++) {

                if (ray.direction.dot(normals[i]) >= 0) continue;

                // produto interno dos 2 vetores
                float cosa = abs(ray.direction.dot(normals[i]));
                float t;
                bool flag = 0;
                Vec3f tmp_p;
                switch (i) {
                    case 0:
                        t = (ray.origin.z - position.z - length.z) / cosa;
                        // Também é necessário determinar em qual direção a origem da luz está no plano
                        if (t < 0) break;
                        tmp_p = ray.getPoint(t);
                        flag = tmp_p.x >= position.x 
                            && tmp_p.x <= position.x + length.x 
                            && tmp_p.y >= position.y 
                            && tmp_p.y <= position.y + length.y;//O intervalo das outras duas coordenadas está em um quadrado?
                        break;
                    case 1:
                        t = (position.z - ray.origin.z) / cosa;
                        if (t < 0) break;
                        tmp_p = ray.getPoint(t);
                        flag = tmp_p.x >= position.x && tmp_p.x <= position.x + length.x && tmp_p.y >= position.y && tmp_p.y <= position.y + length.y;
                        break;
                    case 2:
                        t = (position.y - ray.origin.y) / cosa;
                        if (t < 0) break;
                        tmp_p = ray.getPoint(t);
                        flag = tmp_p.z >= position.z && tmp_p.z <= position.z + length.z && tmp_p.x >= position.x && tmp_p.x <= position.x + length.x;
                        break;
                    case 3:
                        t = (ray.origin.y - position.y - length.y) / cosa;
                        if (t < 0) break;
                        tmp_p = ray.getPoint(t);
                        flag = tmp_p.z >= position.z && tmp_p.z <= position.z + length.z && tmp_p.x >= position.x && tmp_p.x <= position.x + length.x;
                        break;
                    case 4:
                        t = (ray.origin.x - position.x - length.x) / cosa;
                        if (t < 0) break;
                        tmp_p = ray.getPoint(t);
                        flag = tmp_p.z >= position.z && tmp_p.z <= position.z + length.z && tmp_p.y >= position.y && tmp_p.y <= position.y + length.y;
                        break;
                    default:
                        t = (position.x - ray.origin.x) / cosa;
                        if (t < 0) break;
                        tmp_p = ray.getPoint(t);
                        flag = tmp_p.z >= position.z && tmp_p.z <= position.z + length.z && tmp_p.y >= position.y && tmp_p.y <= position.y + length.y;
                        break;
                }
                if (!flag) continue;
                if (t < t1) {
                    t2 = t1;
                    t1 = t;
                }
                else t2 = t;
            }

            if (t1 < INFINITY) return true;
            else return false;
        }

        Vec3f nhit(Vec3f& phit) const {
            if (abs(phit.x - position.x) < smallEnough) return Vec3f(-1.0f, 0.0f, 0.0f);//A espera imprecisa causará problemas
            if (abs(phit.y - position.y) < smallEnough) return Vec3f(0.0f, -1.0f, 0.0f);
            if (abs(phit.z - position.z) < smallEnough) return Vec3f(0.0f, 0.0f, -1.0f);
            if (abs(phit.x - position.x - length.x) < smallEnough) return Vec3f(1.0f, 0.0f, 0.0f);
            if (abs(phit.y - position.y - length.y) < smallEnough) return Vec3f(0.0f, 1.0f, 0.0f);
            else return Vec3f(0.0f, 0.0f, 1.0f);

        }

};


#endif

