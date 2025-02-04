#ifndef CAMERA_H_INCLUDE
#define CAMERA_H_INCLUDE

#include <iostream>
#include <string>
using std::string;

#include <fstream>

#include "Linear.h"
#include "Model.h"

class Camera {
public:
    Camera(string filename, Linear* inc_l, Model* models, int numModels);
    ~Camera();

    double cramers(Face* face, double* ray);
    double cramers2(Face* face, double* ray);
    double calcIntersect(double* ray);
    double throwRay(int x, int y);

    void read(std::ifstream& file);
    void printCamera();
    void calcBasis();
    bool equals(double d1, double d2);

    inline double* getEye() {return eye;}
    inline double* getLook() {return look;}
    inline double* getUp() {return up;}
    // inline double* getBounds() {return bounds;}
    inline double* getRes() {return res;}
    inline double  getD() {return d;}

private:

    double eye [3];
    double look [3];
    double up [3];
    // double bounds [4]; // left, bottom, right, top (bottom, left, top, right a la Beveridge)
    double res [2];
    double* uV;
    double* vV;
    double* wV;

    double left, btm, right, top, d, numModels;
    Linear* l;
    Model* models;
};

#endif