#ifndef FACE_H_INCLUDE
#define FACE_H_INCLUDE

#include "Point.h"
#include <vector>
using std::vector;

class Face{
public:
    Face();
    ~Face();

    void calcNormal();
    void print();
    inline double* getNormal() {return normal;}
    inline vector<Point*> getPoints() {return points;}
    inline void setPoints(vector<Point*> u_points) { points = u_points; }
    
private:
    int numPoints;
    vector<Point*> points;
    double* normal;
};
#endif