#ifndef MODEL_H_INCLUDE
#define MODEL_H_INCLUDE

#include <string>
using std::string;
#include <fstream>

#include <iostream>

#include <vector>
using std::vector;

#include "Point.h"
#include "Face.h"

class Model {
public:
    Model(string filename);
    Model() {};
    ~Model();

    inline vector<Point> getPoints() const {return points;}
    inline vector<Face> getFaces() const {return faces;}
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

    vector<Point> points;
    vector<Face> faces;
    string header;
};

#endif