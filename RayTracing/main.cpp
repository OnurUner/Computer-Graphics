//
//  Created by Onur Can ÜNER on 18.10.2014.
//  Copyright (c) 2014 Onur Can ÜNER. All rights reserved.
//  Single Thread Working Version
//

#include <iostream>
#include <cmath>
#include <stdio.h>
#include <vector>
#include "parseScene.h"
#include "Light.h"
#include "Color.h"
#include "Vec3.h"
#include "Camera.h"
#include "Object.h"
#include "Sphere.h"
#include "Ray.h"
#include "Reader.h"
#include "Triangle.h"
#include "writePPM.h"
using namespace std;

int reflectanceCount;
vector<Object*> objects;
bool isReflective;

Color findPixelColor(vector<Light> lights, Vec3 intersectionPoint, Vec3 camToPixDir, Object *intersectedObj, Color ambientLight ,int depth){

    // ambient shading
    Color pixelColor = intersectedObj->getAmbientColor() * ambientLight;
    Vec3 intersectedObjNormal = (intersectedObj->findNormalWith(intersectionPoint)).normalizeVec();

    if (isReflective) {
        if (depth < reflectanceCount) {
            
            float ndotv = intersectedObjNormal.dotProduct(camToPixDir * (-1.f));
            Vec3 twonnv = (intersectedObjNormal * ndotv) * 2.f;
            Vec3 r = twonnv + (camToPixDir);
            Vec3 reflectionDir = r.normalizeVec();
            Ray reflectRay(intersectionPoint , reflectionDir);
            float tmin = INFINITY;
            Object* closestObj = NULL;
            
            for (int j = 0; j<objects.size() ; j++) {
                float t = objects[j]->findIntersection(reflectRay);
                if (t > 0.001 && t < tmin) {
                    tmin = t;
                    closestObj = objects[j];
                }
            }
            if (closestObj != NULL) {
                Vec3 refIntersectPoint = intersectionPoint + (reflectionDir * tmin);
                int param = depth + 1;
                Color reflectionRayColor = findPixelColor(lights, refIntersectPoint, reflectionDir, closestObj, ambientLight, param);
                pixelColor = pixelColor + (reflectionRayColor * intersectedObj->getReflectanceColor());
            }
        }
    }

    for (int i = 0; i<lights.size(); i++) {
        Vec3 lightDir = (lights[i].getLightPosition() - intersectionPoint).normalizeVec();
        float cosValue = intersectedObjNormal.dotProduct(lightDir);
        float distanceToLight = (lights[i].getLightPosition() - intersectionPoint).getMagnitude();
        const float pi = 3.1415926535897;
        Color newIntensity = lights[i].getLightColor() / (4*pi*distanceToLight*distanceToLight);
        
        if (cosValue > 0) {
            bool shadow = false;
            Ray shadowRay(intersectionPoint, lightDir);

            for (int j = 0; j<objects.size() && shadow == false; j++) {
                float t = objects[j]->findIntersection(shadowRay);
                if (t > 0.001 && t < distanceToLight) {
                    shadow = true;
                    break;
                }
            }
            if (shadow == false) {
                // ambient + diffuse shading
                pixelColor = pixelColor + ((intersectedObj->getDiffuseColor() * newIntensity) * cosValue);

                // specular shading
                Vec3 laddv = lightDir + (camToPixDir * (-1.f));
                Vec3 h = laddv / laddv.getMagnitude();

                float ndoth = intersectedObjNormal.dotProduct(h);
                if(ndoth > 0){
                	float ndothPowe = pow(ndoth, intersectedObj->getSpecularColor().getSpecularExp());
                	pixelColor = pixelColor + (intersectedObj->getSpecularColor() * newIntensity * ndothPowe);
                }
            }
        }
    }
    return pixelColor.clamp();
}


int main(int argc, const char * argv[]) {

	Reader reader;
    reader = parseSceneXML(argv[1]);
    
    // set ambientLight & background
    Color ambientLight = reader.ambientLight;
    Color backgroundColor = reader.backgroundColor;
    isReflective = reader.isReflective;
    reflectanceCount = reader.rayReflectCount;
    
    // fill scene objects
    if (reader.sInterface.size() > 0) {
        reader.fillSpheres(objects);
    }
    if (reader.tInterface.size() > 0) {
        reader.fillTriangles(objects);
    }
    
    // fill light sources
    vector<Light> lights;
    for (int lCount = 0; lCount < reader.pLights.size(); lCount++) {
        lights.push_back(reader.pLights[lCount]);
    }
    
    for (int camCount = 0; camCount < reader.cInterface.size(); camCount++) {
        
        // set image plane
        float left = reader.cInterface[camCount].left;
        float right = reader.cInterface[camCount].right;
        float bottom = reader.cInterface[camCount].bottom;
        float top = reader.cInterface[camCount].top;
        float distance = reader.cInterface[camCount].distance;
        int width = reader.cInterface[camCount].width;
        int height = reader.cInterface[camCount].height;
        
        // set camera
        Camera camera = reader.cInterface[camCount].camera;
        
        int pixIndex = 0;
        
        float tmin, sV, sU;
        float *pixels = new float[width*height*3];

        for (int j = height-1; j>= 0; j--) {
            
            sV = bottom + ((top - bottom)*(j + 0.5)/height);
            
            for (int i = 0; i< width; i++) {
                tmin = INFINITY;
                
                sU = left + ((right - left)*(i + 0.5)/width);
                
                Vec3 S = camera.getCameraPosition() + (camera.getCameraRight() * sU) + (camera.getCameraUp() * sV) + (camera.getCameraDirection() * distance);
                Vec3 camToPixDir = (S - camera.getCameraPosition()).normalizeVec();
                
                Ray ray(camera.getCameraPosition(), camToPixDir);
                Object *intersectedObj = NULL;
                
                for (int k = 0; k<objects.size() ; k++) {
                    float t = objects[k]->findIntersection(ray);

                    if (t > 0 && t < tmin) {
                        tmin = t;
                        intersectedObj = objects[k];
                    }
                }
                if (intersectedObj != NULL) {
                    Vec3 intersectionPoint = camera.getCameraPosition() + (camToPixDir * tmin);
                    Color finalColor = findPixelColor(lights, intersectionPoint, camToPixDir, intersectedObj, ambientLight, 0);
                    
                    pixels[pixIndex] = finalColor.getRed();
                    pixels[pixIndex+1] = finalColor.getGreen();
                    pixels[pixIndex+2] = finalColor.getBlue();
                }
                else{
                    pixels[pixIndex] = backgroundColor.getRed();
                    pixels[pixIndex+1] = backgroundColor.getGreen();
                    pixels[pixIndex+2] = backgroundColor.getBlue();
                }
                pixIndex += 3;
            }
        }
        string fileName = reader.cInterface[camCount].imageName;
        const char *fileNameChar = fileName.c_str();
        writePPM(fileNameChar, width, height, pixels);
    }
   
    return 0;
}
