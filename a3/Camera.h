#ifndef CAMERA_H_INCLUDE
#define CAMERA_H_INCLUDE

#include <iostream>
#include <string>
using std::string;

#include <fstream>

#include "Linear.h"

class Camera {
public:
    Camera(string filename, Linear* inc_l);
    ~Camera();

    double throwRay(int x, int y);

    void read(std::ifstream& file);
    void printCamera();

    inline double* getEye() {return eye;}
    inline double* getLook() {return look;}
    inline double* getUp() {return up;}
    inline double* getBounds() {return bounds;}
    inline double* getRes() {return res;}
    inline double  getD() {return d;}

private:
    double eye [3];
    double look [3];
    double up [3];
    double bounds [4]; // left, bottom, right, top
    double res [2];

    double d;
    Linear* l;
};

#endif