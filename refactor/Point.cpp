#include "Point.h"

#include <iostream>
using std::endl;
using std::cout;

Point::Point() {
    
}

Point::Point(double u_x, double u_y, double u_z) {
    x = u_x;
    y = u_y;
    z = u_z;    
}

double* Point::subtract(Point& p) {
    double* result = new double[3];

    result[0] = x - p.getX();
    result[1] = y - p.getY();
    result[2] = z - p.getZ();

    return result;
}

void Point::setX(double u_x) {
    x = u_x;
}

void Point::setY(double u_y) {
    y = u_y;
}

void Point::setZ(double u_z) {
    z = u_z;
}

void Point::setXYZ(double u_x, double u_y, double u_z) {
    x = u_x;
    y = u_y;
    z = u_z;
}

void Point::translate(double deltaX, double deltaY, double deltaZ) {
    x -= deltaX;
    y -= deltaY;
    z -= deltaZ;
}

void Point::print() {
    cout << "x: " << x << ", y: " << y << ", z: " << z << endl;
}