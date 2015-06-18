/*
 * Ray.h
 *
 *  Created on: Oct 23, 2014
 *      Author: e1949817
 */

#ifndef RAY_H_
#define RAY_H_
#include "Vec3.h"
class Ray{
private:
    Vec3 origin, direction;

public:
    Ray(){
        this->origin = Vec3(0.0, 0.0, 0.0);
        this->direction = Vec3(1.0, 0.0, 0.0);
    }

    Ray(Vec3 org, Vec3 dir){
        this->origin = org;
        this->direction = dir;
    }

    Vec3 getRayOrigin() { return this->origin; }
    Vec3 getRayDirection() { return this->direction; }

};



#endif /* RAY_H_ */
