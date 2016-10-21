#include "Face.h"
#include <iostream>
using std::cout;

Face::Face() {

}

Face::~Face() {
    if(points != nullptr)
        delete [] points;
    if(normal != nullptr)
        delete [] normal;
}

void Face::init(int u_numPoints, Point* u_points) {
    numPoints = u_numPoints;
    points = u_points;
    calcNormal();
}

void Face::calcNormal() {

}

void Face::print() {
    for(int i = 0; i < numPoints; i++) {
        points[i].print();
    }
}