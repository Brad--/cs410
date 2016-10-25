#include "Face.h"
#include <iostream>
using std::cout;

Face::Face() {
    normal = new double[3];
}

Face::~Face() {
    // if(points != nullptr){
    //     // for(int i = 0; i < numPoints; i++)
    //     //     delete points[i];
        
    //     delete [] points;
    // }
    // if(normal != nullptr)
    //     delete [] normal;
    delete points;
    delete normal;
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