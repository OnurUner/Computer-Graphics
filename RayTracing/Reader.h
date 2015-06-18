/*
 * Reader.h
 *
 *  Created on: Oct 23, 2014
 *      Author: e1949817
 */

#ifndef READER_H_
#define READER_H_
#include "Color.h"
#include "Sphere.h"
#include "Light.h"
#include "Camera.h"
#include "Object.h"
#include "Triangle.h"
#include <vector>
#include <iostream>
using namespace std;

struct Material{
	int mid;
	Color ambient, diffuse, specular, reflectance;
};

struct Vertex{
	int vid;
	float X, Y, Z;
};

struct SphereInterface{
    int sphereID, centerID, materialID;
    float radius;
    
};

struct TriangleInterface{
    int trianID, vIndex[3], materialID;
};

struct CameraInterface{
    Camera camera;
    float left,right, bottom, top, distance;
    int width, height;
    string imageName;
};

class Reader{
public:
    vector<SphereInterface> sInterface;
    vector<TriangleInterface> tInterface;
    vector<CameraInterface> cInterface;
    
	vector<Sphere> spheres;
	vector<Material> materials;
	vector<Vertex> vertex;
    vector<Light> pLights;
    
    Color ambientLight;
    Color backgroundColor;
    
    int rayReflectCount;
    bool isReflective;
    
    Reader(){
        this->isReflective = false;
    }
    
	void saveMaterials(int index, float *amb, float *dif, float *spe, float *mir, float phongExp)
	{
        if (mir[0]>0 || mir[1]>0 || mir[2]>0) {
            this->isReflective = true;
        }
		Color ambientColor(amb[0], amb[1], amb[2], 0.0);
		Color diffuseColor(dif[0], dif[1], dif[2], 0.0);
		Color specularColor(spe[0], spe[1], spe[2], phongExp);
		Color reflectanceColor(mir[0], mir[1], mir[2], 0.0);
		Material mat;
		mat.mid = index;
		mat.ambient = ambientColor;
		mat.diffuse = diffuseColor;
		mat.specular = specularColor;
		mat.reflectance = reflectanceColor;

		materials.push_back(mat);
	}

	void saveVertex(int index, float *v)
	{
		Vertex vert;
		vert.vid = index;
		vert.X = v[0];
		vert.Y = v[1];
		vert.Z = v[2];

		vertex.push_back(vert);
	}
    
    void saveTriangle(int index, int vIndex[3], int mIndex)
    {
        TriangleInterface tIn;
        tIn.trianID = index;
        tIn.vIndex[0] = vIndex[0];
        tIn.vIndex[1] = vIndex[1];
        tIn.vIndex[2] = vIndex[2];
        tIn.materialID = mIndex;
        
        tInterface.push_back(tIn);
    }
    
    void saveSphere(int index, int vIndex, int mIndex, float rad)
    {
        SphereInterface sIn;
        sIn.sphereID = index;
        sIn.centerID = vIndex;
        sIn.materialID = mIndex;
        sIn.radius = rad;
        
        sInterface.push_back(sIn);
    }
    
    void savePointLight(int index, float pos[3], float intensity[3])
    {
        Color lightCol(intensity[0], intensity[1], intensity[2], 0.0);
        Vec3 lightPos(pos[0], pos[1], pos[2]);
        Light light(lightPos, lightCol);
        
        pLights.push_back(light);
    }
    
    void saveAmbientLight(float *ambLight)
    {
        Color ambCol(ambLight[0], ambLight[1], ambLight[2], 0.0);
        ambientLight = ambCol;
    }
    
    void saveBackground(float *backCol)
    {
        Color background (backCol[0], backCol[1], backCol[2], 0.0);
        backgroundColor = background;
    }
    
    void saveReflectanceCount(int count) { rayReflectCount = count; }
    
    void saveCamera(int index, float *gaze, float *up, float *pos, float left, float right, float bottom, float top, float distance, int width, int height, string imageName)
    {
        CameraInterface cInt;
        
        Vec3 camdir(gaze[0], gaze[1], gaze[2]);
	camdir = camdir.normalizeVec();

        Vec3 camup(up[0], up[1], up[2]);
	camup = camup.normalizeVec();

        Vec3 campos(pos[0], pos[1], pos[2]);
        Vec3 camright = camup.crossProduct(camdir *(-1.f)).normalizeVec();
        
        Camera cam(campos, camdir, camright, camup);
        cInt.camera = cam;
        
        cInt.left = left;
        cInt.right = right;
        cInt.bottom = bottom;
        cInt.top = top;
        cInt.distance = distance;
        
        cInt.width = width;
        cInt.height = height;
        
        cInt.imageName = imageName;
        
        cInterface.push_back(cInt);
    }
    
    void fillSpheres(vector<Object*> &objects)
    {
        int mid , vid;
        for (int i = 0; i<sInterface.size(); i++) {
            vid = sInterface[i].centerID;
            mid = sInterface[i].materialID;
            
            Material mat;
            Vertex centerPoint;
            
            for (int j = 0; j<materials.size(); j++) {
                if(materials[j].mid == mid){
                    mat = materials[j];
                    break;
                }
            }
            
            for (int j = 0; j<vertex.size(); j++) {
                if(vertex[j].vid == vid){
                    centerPoint = vertex[j];
                    break;
                }
            }
            
            Vec3 cen(centerPoint.X, centerPoint.Y, centerPoint.Z);
            Sphere *sphere = new Sphere(cen, mat.ambient, mat.diffuse, mat.specular, mat.reflectance, sInterface[i].radius);
            objects.push_back(dynamic_cast<Object*>(sphere));
        }
    }
    
    void fillTriangles(vector<Object*> &objects)
    {
        int mid, vid[3];
        
        for (int i = 0; i<tInterface.size(); i++) {
            vid[0] = tInterface[i].vIndex[0];
            vid[1] = tInterface[i].vIndex[1];
            vid[2] = tInterface[i].vIndex[2];
            mid    = tInterface[i].materialID;
            
            Material mat;
            Vertex a, b, c;
            
            for (int j = 0; j<materials.size(); j++) {
                if(materials[j].mid == mid){
                    mat = materials[j];
                    break;
                }
            }
            
            for (int j = 0; j<vertex.size(); j++) {
                if(vertex[j].vid == vid[0])         a = vertex[j];
                else if (vertex[j].vid == vid[1])   b = vertex[j];
                else if (vertex[j].vid == vid[2])   c = vertex[j];
            }
            
            Vec3 apoint(a.X, a.Y, a.Z);
            Vec3 bpoint(b.X, b.Y, b.Z);
            Vec3 cpoint(c.X, c.Y, c.Z);
            
            Triangle *tri = new Triangle(apoint, bpoint, cpoint, mat.ambient, mat.diffuse, mat.specular, mat.reflectance);
            objects.push_back(dynamic_cast<Object*>(tri));
        }
    }
};


#endif /* READER_H_ */
