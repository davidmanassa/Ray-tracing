#ifndef Cube_h
#define Cube_h

#include "GLObject.h"

#define smallEnough 1e-4


class Cube : public Object {
public:
    Vec3f vertex;
    float xLength;
    float yLength;
    float zLength;

    int imageW;//Informação da imagem da textura
    int imageH;
    Vec3f* imageBuffer;//cor

    Cube(Vec3f v,
        float xl,
        float yl,
        float zl,
        Vec3f sc,
        float trans,
        Vec3f ec = BLACK,
        Material material = Material(0.3, 1, 0.9, 6)) : vertex(v), xLength(xl), yLength(yl), zLength(zl), Object(sc, trans, ec, material) {}

    Vec3f getTextureColor(Vec3f& phit) const {
        return WHITE;
    }

    bool intersect(Ray& ray, float& t1, float& t2) const {
        //Julgue cruzamento externo

        Vec3f nTop(0.0f, 0.0f, 1.0f);//O vetor normal das seis faces do cubo
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
            float cosa = abs(ray.direction.dot(normals[i]));
            float t;
            bool flag = 0;
            Vec3f tmp_p;
            switch (i) {
            case 0:
                t = (ray.origin.z - vertex.z - zLength) / cosa;// Também é necessário determinar em qual direção a origem da luz está no plano
                if (t < 0) break;
                tmp_p = ray.getPoint(t);
                flag = tmp_p.x >= vertex.x && tmp_p.x <= vertex.x + xLength && tmp_p.y >= vertex.y && tmp_p.y <= vertex.y + yLength;//O intervalo das outras duas coordenadas está em um quadrado?
                break;
            case 1:
                t = (vertex.z - ray.origin.z) / cosa;
                if (t < 0) break;
                tmp_p = ray.getPoint(t);
                flag = tmp_p.x >= vertex.x && tmp_p.x <= vertex.x + xLength && tmp_p.y >= vertex.y && tmp_p.y <= vertex.y + yLength;
                break;
            case 2:
                t = (vertex.y - ray.origin.y) / cosa;
                if (t < 0) break;
                tmp_p = ray.getPoint(t);
                flag = tmp_p.z >= vertex.z && tmp_p.z <= vertex.z + zLength && tmp_p.x >= vertex.x && tmp_p.x <= vertex.x + xLength;
                break;
            case 3:
                t = (ray.origin.y - vertex.y - yLength) / cosa;
                if (t < 0) break;
                tmp_p = ray.getPoint(t);
                flag = tmp_p.z >= vertex.z && tmp_p.z <= vertex.z + zLength && tmp_p.x >= vertex.x && tmp_p.x <= vertex.x + xLength;
                break;
            case 4:
                t = (ray.origin.x - vertex.x - xLength) / cosa;
                if (t < 0) break;
                tmp_p = ray.getPoint(t);
                flag = tmp_p.z >= vertex.z && tmp_p.z <= vertex.z + zLength && tmp_p.y >= vertex.y && tmp_p.y <= vertex.y + yLength;
                break;
            default:
                t = (vertex.x - ray.origin.x) / cosa;
                if (t < 0) break;
                tmp_p = ray.getPoint(t);
                flag = tmp_p.z >= vertex.z && tmp_p.z <= vertex.z + zLength && tmp_p.y >= vertex.y && tmp_p.y <= vertex.y + yLength;
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
        if (abs(phit.x - vertex.x) < smallEnough) return Vec3f(-1.0f, 0.0f, 0.0f);//A espera imprecisa causará problemas
        if (abs(phit.y - vertex.y) < smallEnough) return Vec3f(0.0f, -1.0f, 0.0f);
        if (abs(phit.z - vertex.z) < smallEnough) return Vec3f(0.0f, 0.0f, -1.0f);
        if (abs(phit.x - vertex.x - xLength) < smallEnough) return Vec3f(1.0f, 0.0f, 0.0f);
        if (abs(phit.y - vertex.y - yLength) < smallEnough) return Vec3f(0.0f, 1.0f, 0.0f);
        else return Vec3f(0.0f, 0.0f, 1.0f);

    }

};


#endif

