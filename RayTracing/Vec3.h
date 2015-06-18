/*
 * Vec3.h
 *
 *  Created on: Oct 23, 2014
 *      Author: e1949817
 */

#ifndef VEC3_H_
#define VEC3_H_

class Vec3{
private:
    float x, y, z, magnitude;

public:
    Vec3(){
        this->x = 0.0;
        this->y = 0.0;
        this->z = 0.0;
        this->magnitude = 0.0;
    }

    Vec3(float X, float Y, float Z){
        this->x = X;
        this->y = Y;
        this->z = Z;
        this->magnitude = sqrt((x*x) + (y*y) + (z*z));
    }

    // setter
    void setVecX(float X) { this->x = X; }
    void setVecY(float Y) { this->y = Y; }
    void setVecZ(float Z) { this->z = Z; }

    // getter
    float getVecX()const { return this->x; }
    float getVecY()const { return this->y; }
    float getVecZ()const { return this->z; }
    float getMagnitude()const { return this->magnitude; }

    // vector operations
    Vec3  normalizeVec() {
        return Vec3(x / magnitude, y / magnitude, z / magnitude);
    }

    float dotProduct(Vec3 vec){
        return (this->x * vec.getVecX()) + (this->y * vec.getVecY()) + (this->z * vec.getVecZ());
    }

    Vec3 crossProduct(Vec3 vec){
        float cX = (y * vec.getVecZ()) - (z * vec.getVecY());
        float cY = (x * vec.getVecZ()) - (z * vec.getVecX());
        float cZ = (x * vec.getVecY()) - (y * vec.getVecX());

        return Vec3(cX, -cY, cZ);
    }

    Vec3 operator*(const int& scaler){
        float cX = x * scaler;
        float cY = y * scaler;
        float cZ = z * scaler;

        return Vec3(cX, cY, cZ);
    }

    Vec3 operator*(const float& scaler){
        float cX = x * scaler;
        float cY = y * scaler;
        float cZ = z * scaler;

        return Vec3(cX, cY, cZ);
    }

    Vec3 operator/(const float& scaler){
        float cX = x / scaler;
        float cY = y / scaler;
        float cZ = z / scaler;

        return Vec3(cX, cY, cZ);
    }

    Vec3 operator+(const Vec3& vec){
        float cX = x + vec.getVecX();
        float cY = y + vec.getVecY();
        float cZ = z + vec.getVecZ();

        return Vec3(cX, cY, cZ);
    }

    Vec3 operator-(const Vec3& vec){
        float cX = x - vec.getVecX();
        float cY = y - vec.getVecY();
        float cZ = z - vec.getVecZ();

        return Vec3(cX, cY, cZ);
    }
};



#endif /* VEC3_H_ */
