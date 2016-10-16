#include "Scene.h"

#include <algorithm>
using std::max;
#include <iostream>
using std::cout;
using std::endl;

Scene::Scene() { 
    tmin = -1;
    tmax = -1;
}

Scene::~Scene() { }

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