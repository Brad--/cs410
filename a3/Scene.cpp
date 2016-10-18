#include "Scene.h"

#include <string>
using std::string;
#include <algorithm>
using std::max;
#include <iostream>
using std::cout;
using std::endl;
using std::ofstream;

Scene::Scene() { 
    tmin = 999999;
    tmax = -2;
}

Scene::~Scene() {
    if(image != nullptr){
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
            if(dist != -1){ // Don't set the minimum with the non-intersections
                if(dist < tmin)
                    tmin = dist;
                if (dist > tmax)
                    tmax = dist;
            }
            image[x][y] = dist;
        }
    }
}

// Write out the image as relative depth in pgm format
void Scene::depthWrite(string filename) {
    ofstream outfile(filename);

    outfile << "P3" << endl;
    outfile << resX << " " << resY << " 255" << endl;

    double* color;
    for(int r = 0; r < resX; r++) {
        for(int c = 0; c < resY; c++) {
            color = distToDepth(image[r][c]);
            outfile << color[0] << " " << color[1] << " " << color[2] << " ";
            delete [] color;
        }
        outfile << endl;
    }

    outfile.close();
}

double* Scene::distToDepth(double d) {
    if(tmin == -2 && tmax == 999999){
        cout << "You can't generate relative distance without a relative minimum and maximum. Throw your rays first!" << endl;
        throw -1;
    }

    // If there wasn't an intersection, return the background color
    if(d == -1) {
        double* background = new double[3];
        for(int i = 0; i < 3; i++)
            background[i] = 239;
        return background;
    }

    double* color = new double[3];
    double ratio = (2 * (d - tmin)) / (tmax - tmin);
    color[0] = max((double)0, 255 * (double)(1 - ratio)); // r
    color[2] = max((double)0, 255 * (double)(ratio - 1)); // b
    color[1] = 255 - color[1] - color[2]; // g

    return color;
}