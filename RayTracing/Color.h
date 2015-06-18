/*
 * Color.h
 *
 *  Created on: Oct 23, 2014
 *      Author: e1949817
 */

#ifndef COLOR_H_
#define COLOR_H_

#include <cmath>
#include <vector>
class Color{
private:
    float red, green, blue, specularExp;
public:
    Color(){
        this->red = 1.0;
        this->green = 1.0;
        this->blue = 1.0;
        this->specularExp = 0.0;
    }

    Color(float r, float g, float b, float s){
        this-> red = r;
        this-> green = g;
        this-> blue = b;
        this-> specularExp = s;
    }

    // setter
    void setRed(float r)     { this->red = r; }
    void setGreen(float g)   { this->green = g; }
    void setBlue(float b)    { this->blue = b; }
    void setSpecularExp(float s) { this->specularExp = s; }

    // getter
    float getRed()const      { return  this->red; }
    float getGreen()const    { return  this->green; }
    float getBlue()const     { return  this->blue; }
    float getSpecularExp()const  { return  this->specularExp; }

    Color operator/(const float& val){ return Color(red/val, green/val, blue/val, specularExp); }
    Color operator*(const float& val){ return Color(red*val, green*val, blue*val, specularExp); }
    Color operator*(const Color& val){ return Color(red*val.getRed(), green*val.getGreen(), blue*val.getBlue(), specularExp); }
    Color operator+(const Color& val){ return Color(red+val.getRed(), green+val.getGreen(), blue+val.getBlue(), specularExp); }

    Color clamp(){
        float total = red + green + blue;
        float overflow = total - 765.f;

        if (overflow > 0) {
            red = red + overflow * (red / total);
            green = green + overflow * (green / total);
            blue = blue + overflow * (blue / total);
        }
        if (red > 255) red = 255;
        if (green > 255) green = 255;
        if (blue > 255) blue = 255;

        if (red < 0) red = 0;
        if (green < 0) green = 0;
        if (blue < 0) blue = 0;
        return Color(red, green, blue , specularExp);
    }

};




#endif /* COLOR_H_ */
