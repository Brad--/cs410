#include "Point.h"

Point::Point(double u_x, double u_y, double u_z) {
    x = u_x;
    y = u_y;
    z = u_z;    
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