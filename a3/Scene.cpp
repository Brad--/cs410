#include "Scene.h"

#include <algorithm>
using std::max;
#include <iostream>
using std::cout;
using std::endl;

Scene::Scene() { 
    tmin = 999999;
    tmax = -1;
}

Scene::~Scene() { 
    if(image != NULL){
        for(int i = 0; i < resX; i++)
            delete [] image[i];
        delete[] image;
    }
}

void Scene::allocateImage() {
    image = new double* [resX];
    for(int i = 0; i < resX; i++)
        image[i] = new double [resY];
}

void Scene::genIntersections(Camera * cam){
    double* res = cam->getRes();
    resX = res[0];
    resY = res[1];
    allocateImage();
    double dist = -1;
    for(int x = 0; x < resX; x++){
        for(int y = 0; y < resY; y++) {
            dist = cam->throwRay(x, y);
            if(dist < tmin)
                tmin = dist;
            if (dist > tmax)
                tmax = dist;
            image[x][y] = dist;
        }
    }

    // maybe delete res, idk though
}

double* Scene::distToDepth(double d) {
    if(tmin == -1 && tmax == -1){
        cout << "You can't generate relative distance without a relative minimum and maximum. Throw your rays first!" << endl;
        throw -1;
    }

    double* color = new double[3];
    double ratio = (2 * (d - tmin)) / (tmax - tmin);
    color[0] = max((double)0, 255 * (double)(1 - ratio));
    color[1] = max((double)0, 255 * (double)(ratio - 1));
    color[2] = 255 - color[1] - color[2];

    return color;
}