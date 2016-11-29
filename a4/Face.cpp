#include "Face.h"
#include <iostream>
using std::cout;

Face::Face() {
}

Face::~Face() {
}

void Face::calcNormal() {

}

void Face::print() {
    for(unsigned int i = 0; i < points.size(); i++) {
        points[i]->print();
    }
}