#ifndef MODEL_H_INCLUDE
#define MODEL_H_INCLUDE

#include <string>
using std::string;
#include <fstream>

#include <iostream>

#include "Point.h"
#include "Face.h"

class Model {
public:
    Model(string filename);
    Model() {};
    ~Model();

    inline Point* getPoints() const {return points;}
    inline Face* getFaces() const {return faces;}
    inline double getNumFaces() const {return numFaces;}

    bool  read(std::ifstream& file);
    bool  write(string filename, string ext) const;
    void  init(string filename);
    void  addPoint(Point point);
    void  normalize();
    void  whiten();
    void  center();
    void  translate(double x, double y, double z);
    Point avgPoint() const;
    Point stdDev() const;

    void printPoints();
    void printBoundingBox();
    void printAll();
private:
    int numPoints, numFaces;

    Point* points;
    Face* faces;
    string header;
};

#endif