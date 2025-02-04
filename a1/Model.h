#ifndef MODEL_H_INCLUDE
#define MODEL_H_INCLUDE

#include <string>
using std::string;
#include <fstream>

// Get rid of this later
#include <iostream>

#include "Point.h"

class Model {
public:
    Model(string filename);
    ~Model();

    inline Point* getPoints() const {return points;}

    bool  read(std::ifstream& file);
    bool  write(string filename, string ext) const;
    void  addPoint(Point point);
    void  normalize();
    void  whiten();
    void  center();
    Point avgPoint() const;
    Point stdDev() const;

    void printPoints();
    void printBoundingBox();
    void printAll();
private:
    int numPoints, numFaces;

    Point* points;
    string header, faces;
};

#endif