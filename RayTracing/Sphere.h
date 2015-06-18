/*
 * Sphere.h
 *
 *  Created on: Oct 23, 2014
 *      Author: e1949817
 */

#ifndef SPHERE_H_
#define SPHERE_H_
#include "Object.h"

class Sphere: public Object{
private:
    Vec3 center;
    float radius;

public:
    Sphere():Object(){
        this->center = Vec3(0.0, 0.0, 0.0);
        this->radius = 1.0;
    }

    Sphere(Vec3 cen, Color ambient, Color diffuse, Color specular, Color reflectance, float rad): Object(ambient, diffuse, specular, reflectance){
        this->center = cen;
        this->radius = rad;
    }

    // getter
    Vec3 getCenter() { return this->center; }
    float getRadius() { return this->radius; }

    // virtual

    virtual Vec3 findNormalWith(Vec3 p){
        // center of sphere - p point

        Vec3 norm = p - center;
        return norm.normalizeVec();
    }
    virtual float findIntersection(Ray ray) {
        // analytic solution
        Vec3 rayOrigin = ray.getRayOrigin();
        Vec3 camToCenter = rayOrigin - center;

        float t0, t1;

        float A = 1.0;
        float B = 2 * (ray.getRayDirection().dotProduct(camToCenter));
        float C = camToCenter.dotProduct(camToCenter) - (radius * radius);

        float discriminant = (B * B) - (4 * C);

        if (discriminant < 0) {
            return -1.0;
        }
        else if (discriminant == 0) {
            t0 = (-B)/(2*A);
            t1 = t0;
            return t0;
        }
        else {
            t0 = (-B - sqrt(discriminant)) / 2;
            t1 = (-B + sqrt(discriminant)) / 2;
            float tmp;
            if (t0 > t1){
                tmp = t0;
                t0 = t1;
                t1 = tmp;
            }
            return t0;
        }
    }

};



#endif /* SPHERE_H_ */
