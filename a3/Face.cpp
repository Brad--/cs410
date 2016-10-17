#include <Face.h>

Face::Face() {

}

Face::~Face() {
    if(points != nullptr)
        delete [] points;
    if(normal != nullptr)
        delete [] normal;
}

void Face::calcNormal() {

}