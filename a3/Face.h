#ifndef FACE_H_INCLUDE
#define FACE_H_INCLUDE

#include "Point.h"

class Face{
public:
    Face();
    ~Face();

    void init(int u_numPoints, Point* u_points);
    void calcNormal();
    void print();
    inline double* getNormal() {return normal;}
    inline Point* getPoints() {return points;}
    
private:
    int numPoints;
    Point* points;
    double* normal;
};
#endif