/*
 * Light.h
 *
 *  Created on: Oct 23, 2014
 *      Author: e1949817
 */

#ifndef LIGHT_H_
#define LIGHT_H_
#include "Color.h"
#include "Vec3.h"

class Light{
private:
    Vec3 position;
    Color color;

public:
    Light(){
        this->position = Vec3(0.0, 0.0, 0.0);
        this->color = Color();
    }

    Light(Vec3 pos, Color col){
        this->position = pos;
        this->color = col;
    }

    Vec3 getLightPosition() { return this->position; }
    Color getLightColor() { return this->color; }

};



#endif /* LIGHT_H_ */
