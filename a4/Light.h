#ifndef LIGHT_H_INCLUDE
#define LIGHT_H_INCLUDE

#include "Point.h"

class Light {
public:
    Light();
    Light(Point p, Point c);

    void setup(Point p, Point c);
    void printLight();

    inline double getX(){return point.getX();}
    inline double getY(){return point.getY();}
    inline double getZ(){return point.getZ();}

    inline double getR(){return color.getX();}
    inline double getG(){return color.getY();}
    inline double getB(){return color.getZ();}

    inline void setW(double theW){w = theW;}
private:
    Point point;
    Point color;

    double w;
};

#endif