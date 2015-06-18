/*
 * Camera.h
 *
 *  Created on: Oct 23, 2014
 *      Author: e1949817
 */

#ifndef CAMERA_H_
#define CAMERA_H_
#include "Vec3.h"

class Camera{
private:
    Vec3 camPosition, camDirection, camRight, camUp;

public:
    Camera(){
        this->camPosition = Vec3(0.0, 0.0, 0.0);
        this->camDirection = Vec3(0.0, 0.0, 1.0);
        this->camRight = Vec3(0.0, 0.0, 0.0);
        this->camUp = Vec3(0.0, 0.0, 0.0);
    }

    Camera(Vec3 position, Vec3 direction, Vec3 right, Vec3 up){
        this->camPosition = position;
        this->camDirection = direction;
        this->camRight = right;
        this->camUp = up;
    }

    Vec3 getCameraPosition() { return this->camPosition; }
    Vec3 getCameraDirection() { return this->camDirection.normalizeVec(); }
    Vec3 getCameraRight() { return this->camRight.normalizeVec(); }
    Vec3 getCameraUp() { return this->camUp.normalizeVec(); }
};



#endif /* CAMERA_H_ */
