#include "Light.h"

#include <iostream>
using std::cout;
using std::endl;

Light::Light(){ }

Light::Light(Point p, Point c) {
    point = p;
    color = c;
}

void Light::setup(Point p, Point c) {
    point = p;
    color = c;
}

void Light::printLight() {
    cout << "Point: ";
    point.print();
    cout << "Color: ";
    color.print();
}