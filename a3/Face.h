#ifndef FACE_H_INCLUDE
#define FACE_H_INCLUDE

#include "Point.h"

class Face{
public:
    Face();
    ~Face();

    void calcNormal();
    inline double* getNormal() {return normal;}
    
private:
    int numPoints;
    Point* points;
    double* normal;
};
#endif