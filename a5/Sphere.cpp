#include "Sphere.h"
#include <iostream>
using std::cout;
using std::endl;

void Sphere::printSphere() {
    cout << "Point: ";
    loc.print();
    cout << "Color: ";
    color.print();
    cout << "Radius: " << radius << endl;
}