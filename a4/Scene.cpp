#include "Scene.h"

#include <cstring>
#include <string>
using std::string;  
using std::stod;
#include <algorithm>
using std::max;
#include <iostream>
using std::cout;
using std::endl;
using std::ofstream;


Scene::Scene(string filename, Linear* inc_l) { 
    tmin = 999999;
    tmax = -2;

    cam = Camera(inc_l);

    char cFilename [256];
    std::strcpy(cFilename, filename.c_str());

    ifstream file(cFilename);
    cam.read(file);
    cam.calcBasis();

    read(file);
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

void Scene::genDistances(){
    double* res = cam.getRes();
    width = res[0];
    height = res[1];
    allocateImage();
    double dist = -1;

    for(int row = 0; row < height; row++) {
        for(int col = 0; col < width; col++) {
            dist = cam.throwRay(row, col);
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
    for(int r = 0; r < height; r++) {
        for(int c = 0; c < width - 1; c++) {
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

void Scene::read(ifstream& file) {
    if (file.fail()) {
        file.clear();
        cout << "Scene read is dead, RIP" << endl;
        return;
    }
    string curr;
    char* token;
    char* currChars;

    while(!file.eof()) {
        getline(file, curr);
        currChars = const_cast<char*>(curr.c_str());
        token = strtok(currChars, " ");

        if (strcmp(token, "ambient") == 0) {
            for(int i = 0; i < 3; i++) {
                token = strtok(NULL, " ");
                ambient[i] = stod(token);
            }
        }
        else if (strcmp(token, "light") == 0) {
            Point point;
            token = strtok(NULL, " ");
            point.setX(stod(token));
            token = strtok(NULL, " ");
            point.setY(stod(token));
            token = strtok(NULL, " ");
            point.setZ(stod(token));
            token = strtok(NULL, " ");
            double w = stod(token);
            Point color;
            token = strtok(NULL, " ");
            color.setX(stod(token));
            token = strtok(NULL, " ");
            color.setY(stod(token));
            token = strtok(NULL, " ");
            color.setZ(stod(token));

            Light light = Light(point, color);
            light.setW(w);
            lights.push_back(light);
        }
        else if (strcmp(token, "sphere") == 0) {
            Point loc;
            token = strtok(NULL, " ");
            loc.setX(stod(token));
            token = strtok(NULL, " ");
            loc.setY(stod(token));
            token = strtok(NULL, " ");
            loc.setZ(stod(token));
            token = strtok(NULL, " ");
            double rad = stod(token);
            Point col;
            token = strtok(NULL, " ");
            col.setX(stod(token));
            token = strtok(NULL, " ");
            col.setY(stod(token));
            token = strtok(NULL, " ");
            col.setZ(stod(token));

            Sphere sphere = Sphere(loc, rad, col);
            spheres.push_back(sphere);
        }
        else if (strcmp(token, "model") == 0) {
            double translation[3];
            token = strtok(NULL, " ");
            translation[0] = stod(token);
            token = strtok(NULL, " ");
            translation[1] = stod(token);
            token = strtok(NULL, " ");
            translation[2] = stod(token);
            // throwing away the rotation values
            for(int i = 0; i < 4; i++)
                token = strtok(NULL, " ");

            token = strtok(NULL, " ");
            string filename = token;
            Model m = Model();
            ifstream file (filename);
            m.read(file);
            m.translate(translation[0], translation[1], translation[2]);
            cam.addModel(&m);

            // Yet to create the model / add it to the camera's list of models
        }
        else {
            // Do nothing
        }
    }
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

void Scene::printScene() {
    cout << "Camera:" << endl;
    cam.printCamera();
    cout << endl;
    cout << "Ambient Light: " << ambient[0] << ", " << ambient[1] << ", " << ambient[2] << endl;
    for(unsigned int i = 0; i < lights.size(); i++) {
        cout << "Light " << i << ": ";
        lights[i].printLight();
    } 
}