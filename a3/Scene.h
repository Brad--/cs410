#ifndef SCENE_H_INCLUDE
#define SCENE_H_INCLUDE

#include "Camera.h"
#include <string>

class Scene{
public:
    Scene();
    ~Scene();

    void genDistances(Camera * cam);
    void depthWrite(std::string filename);

    double* distToDepth(double d);
private:

    void allocateImage();
    // Maximum and Minimum distance in the scene
    double tmin;
    double tmax;

    double** image;
    int width, height;
    
};

#endif