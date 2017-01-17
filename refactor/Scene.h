#ifndef SCENE_H_INCLUDE
#define SCENE_H_INCLUDE

#include "Camera.h"
#include "Light.h"
#include "Sphere.h"
#include "Material.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using std::vector;

class Scene{
public:
    Scene(string filename, Linear* inc_l);
    ~Scene();

    void genDistances();
    void write(std::string filename);
    void read(std::ifstream& file);
    void readMat(std::ifstream& file);

    void printScene();

    double* getColor(double pos);
    double* distToDepth(double d);
private:

    void allocateImage();

    Camera cam;

    double ambient[3];
    vector<Light> lights;
    vector<Sphere> spheres;
    vector<Material> materials;

    // Maximum and Minimum distance in the scene
    double tmin;
    double tmax;

    double** image;
    vector<double*> colors;
    int width, height;
};

#endif