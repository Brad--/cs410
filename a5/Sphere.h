#ifndef SPHERE_H_INCLUDE
#define SPHERE_H_INCLUDE

#include "Point.h"

class Sphere{
public:
    Sphere();
    inline Sphere(Point p, double rad, Point col)
    :loc(p), color(col), radius(rad) { };

    inline double getX() {return loc.getX();}
    inline double getY() {return loc.getY();}
    inline double getZ() {return loc.getZ();}

    inline Point getLoc() {return loc;}
    inline Point getColor() {return color;}

    inline double getR() {return color.getX();}
    inline double getG() {return color.getY();}
    inline double getB() {return color.getZ();}

    inline double getRadius() {return radius;}

    void printSphere();

private:
    Point loc, color;
    double radius;
};

#endif