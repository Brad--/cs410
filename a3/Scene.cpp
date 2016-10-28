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
        for(int i = 0; i < height; i++)
            delete [] image[i];
        delete[] image;
    }
}

void Scene::allocateImage() {
    image = new double* [height];
    for(int i = 0; i < height; i++)
        image[i] = new double [width];
}

void Scene::genDistances(Camera* cam){
    double* res = cam->getRes();
    width = res[0];
    height = res[1];
    allocateImage();
    double dist = -1;

    for(int row = 0; row < height; row++) {
        for(int col = 0; col < width; col++) {
            dist = cam->throwRay(row, col);
            if(dist != -1) {
                if(dist < tmin)
                    tmin = dist;
                if (dist > tmax)
                    tmax = dist;
            }
            image[row][col] = dist;
        }
    }

    // for(int x = 0; x < resX; x++){
    //     for(int y = 0; y < resY; y++) {
    //         cout << image[x][y] << " ";
    //     }
    //     cout << endl;
    // }

    // cout << "tmin: " << tmin << ", tmax: " << tmax << endl;
}

void Scene::jankyWrite(string filename) {
    // Still doesn't work, but fuck it, it isn't seg faulting
    ofstream outfile(filename);
    outfile << "P3" << endl;
    outfile << width << " " << height << " 255" << endl;

    double* color;
        for(int c = 0; c < width - 1; c++) {
    for(int r = 0; r < height; r++) {
            color = distToDepth(image[r][c]);
            outfile << (int)color[0] << " " << (int)color[1] << " " << (int)color[2] << " ";
            delete [] color;
        }
        outfile << endl;
    }
    outfile.close();
}

// Write out the image as relative depth in pgm format
void Scene::depthWrite(string filename) {
    if(width != height) {
        // This is janky as fuck
        jankyWrite(filename);
        return;
    }
    ofstream outfile(filename);
    outfile << "P3" << endl;
    outfile << width << " " << height << " 255" << endl;

    double* color;
    for(int r = height - 1; r >= 0; r--) {
        for(int c = 0; c < width; c++) {
            color = distToDepth(image[c][r]);
            outfile << (int)color[0] << " " << (int)color[1] << " " << (int)color[2] << " ";
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
    double ratio = (double)(2 * (d - tmin)) / (double)(tmax - tmin);
    color[0] = max((double)0, 255 * (double)(1 - ratio)); // r
    color[2] = max((double)0, 255 * (double)(ratio - 1)); // b
    color[1] = 255 - (int)color[0] - (int)color[2]; // g

    return color;
}