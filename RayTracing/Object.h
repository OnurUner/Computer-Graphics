/*
 * Object.h
 *
 *  Created on: Oct 23, 2014
 *      Author: e1949817
 */

#ifndef OBJECT_H_
#define OBJECT_H_
#include "Color.h"
#include "Ray.h"
#include "Vec3.h"

class Object{
protected:
    Color ambientColor, diffuseColor, specularColor, reflectanceColor;

public:
    Object(){
        this->ambientColor = Color(0.0, 0.0, 0.0, 0.0);
        this->diffuseColor = Color(0.0, 0.0, 0.0, 0.0);
        this->specularColor = Color(0.0, 0.0, 0.0, 0.0);
        this->reflectanceColor = Color(0.0, 0.0, 0.0, 0.0);
    }

    Object(Color ambient, Color diffuse, Color specular, Color reflectance){
        this->ambientColor = ambient;
        this->diffuseColor = diffuse;
        this->specularColor = specular;
        this->reflectanceColor = reflectance;
    }

    Color getAmbientColor() { return this->ambientColor; }
    Color getDiffuseColor() { return this->diffuseColor; }
    Color getSpecularColor() { return this->specularColor; }
    Color getReflectanceColor() { return this->reflectanceColor; }

    virtual float findIntersection(Ray ray) { return -1.0; }
    virtual Vec3 findNormalWith(Vec3 p) { return Vec3(0.0, 0.0, 0.0); }
};



#endif /* OBJECT_H_ */
