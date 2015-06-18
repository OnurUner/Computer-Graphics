//
//  Triangle.h
//  ceng477-hw1
//
//  Created by Onur Can ÜNER on 24.10.2014.
//  Copyright (c) 2014 Onur Can ÜNER. All rights reserved.
//

#ifndef _Triangle_h
#define _Triangle_h
#include "Object.h"
#include "Vec3.h"

class Triangle: public Object{
private:
    Vec3 aPoint, bPoint, cPoint, normal;
    Vec3 edgeAC , edgeBA , edgeCB;
    float distance;
    
public:
   
    Triangle(Vec3 A, Vec3 B, Vec3 C, Color ambient, Color diffuse, Color specular, Color reflectance): Object(ambient, diffuse, specular, reflectance){
        this->aPoint = A;
        this->bPoint = B;
        this->cPoint = C;
        
        // find normal
        Vec3 atoc = cPoint - aPoint;
        Vec3 atob = bPoint - aPoint;
        this->normal = (atob.crossProduct(atoc)).normalizeVec();
        
        // find distance to triangle origin
        this->distance = this->normal.dotProduct(aPoint);
        
        // find edges
        this->edgeAC = (aPoint - cPoint).normalizeVec();
        this->edgeBA = (bPoint - aPoint).normalizeVec();
        this->edgeCB = (cPoint - bPoint).normalizeVec();
    }
    
    // getter
    float getDistance() { return this->distance; }
    Vec3 getAPoint() { return this->aPoint; }
    Vec3 getBPoint() { return this->bPoint; }
    Vec3 getCPoint() { return this->cPoint; }
    Vec3 getNormal() { return this->normal; }
    
    // virtual
    virtual Vec3 findNormalWith(Vec3 p){
        return this->normal;
    }
    virtual float findIntersection(Ray ray) {
        float denom = (ray.getRayDirection()).dotProduct(this->normal);
        
        if (denom != 0) {
            // ray trinagle intersection

            float t = ((distance) - (this->normal.dotProduct(ray.getRayOrigin())))/ denom;
            
            Vec3 camToPlanePoint = ray.getRayOrigin() + (ray.getRayDirection() * t);
            
            Vec3 ra = (camToPlanePoint - aPoint).normalizeVec();
            Vec3 rc = (camToPlanePoint - cPoint).normalizeVec();
            Vec3 rb = (camToPlanePoint - bPoint).normalizeVec();
            
            Vec3 v1 = edgeAC.crossProduct(rc);
            Vec3 v2 = edgeCB.crossProduct(rb);
            Vec3 v3 = edgeBA.crossProduct(ra);

            float test1 = v1.dotProduct(v2);
            float test2 = v2.dotProduct(v3);

            if (test1 >= -1e-10 && test2 >= -1e-10){
                return t;
            }
            else
                return -1;
        }
        else{
            // ray and triangle paralel
            return -1;
        }
    }
};

#endif
