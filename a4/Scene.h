#ifndef SCENE_H_INCLUDE
#define SCENE_H_INCLUDE

#include "Camera.h"
#include <string>
#include <iostream>
#include <fstream>

class Scene{
public:
    Scene(string filename, Linear* inc_l, Model* inc_models, int inc_numModels);
    ~Scene();

    void genDistances();
    void depthWrite(std::string filename);
    void jankyWrite(std::string filename);
    void read(std::ifstream& file);

    double* distToDepth(double d);
private:

    void allocateImage();

    Camera cam;

    // Maximum and Minimum distance in the scene
    double tmin;
    double tmax;

    double** image;
    int width, height;
};

#endif