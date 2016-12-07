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
    colors.reserve(height * width);
}

void Scene::genDistances(){
    double* res = cam.getRes();
    width = res[0];
    height = res[1];
    allocateImage();
    // double dist = -1;
    double* color;
    double* black = new double[3];
    black[0] = 0; black[1] = 0; black[2] = 0;

    for(int row = height - 1; row >= 0; row--) {
        for(int col = 0; col < width; col++) {
            color = cam.throwRay(row, col);
            if(color[0] == -1){
                image[row][col] = -1;
                colors.push_back(black);
            }
            else {
                image[row][col] = colors.size();
                colors.push_back(color);
            }
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
    ofstream outfile(filename);
    outfile << "P3" << endl;
    outfile << width << " " << height << " 255" << endl;

    double* color;
    for(unsigned int i = 0; i < colors.size(); i++) {
        color = colors[i];
        outfile << (int)color[0] << " " << (int)color[1] << " " << (int)color[2] << " ";
    }
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
            // color = getColor(image[c][r]);
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
    string matFilename;
    string curr;
    char* token;
    char* currChars;

    while(!file.eof()) {
        getline(file, curr);
        currChars = const_cast<char*>(curr.c_str());
        token = strtok(currChars, " ");
        if(token != nullptr){
            if (strcmp(token, "ambient") == 0) {
                for(int i = 0; i < 3; i++) {
                    token = strtok(NULL, " ");
                    ambient[i] = stod(token);
                }
                cam.setAmbient(ambient[0], ambient[1], ambient[2]);
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

                Light* light = new Light(point, color);
                light->setW(w);
                // lights.push_back(light);
                cam.addLight(light);
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

                Sphere* sphere = new Sphere(loc, rad, col);
                cam.addSphere(sphere);
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
                Model *m = new Model();
                ifstream file (filename);
                m->read(file, materials);
                m->translate(translation[0], translation[1], translation[2]);    
                cam.addModel(m);
            }
            else if (strcmp(token, "mtllib") == 0) {
                // token = strtok(NULL, " ");
                // ifstream matfile(token);

                // readMat(matfile);
            }
            else {
                // Do nothing
            }
        }
    }
    
}

void Scene::readMat(ifstream& file) {
    if(file.fail()){
        file.clear();
        cout << "Matfile is RIP" << endl;
        return;
    }

    string matFilename;
    string curr;
    char* token;
    char* currChars;

    while(!file.eof()) {
        getline(file, curr);
        currChars = const_cast<char*>(curr.c_str());
        token = strtok(currChars, " ");
        Material material;

        if (strcmp(token, "newmtl") == 0) {
            token = strtok(NULL, " ");
            material = Material(token);
        }
        else if (strcmp(token, "Kd") == 0) {
            double r,g,b;
            token = strtok(NULL, " ");
            r = stod(token);
            token = strtok(NULL, " ");
            g = stod(token);
            token = strtok(NULL, " ");
            b = stod(token);
            material.setDiffuse(r, g, b);
        } 
        else if (strcmp(token, "Ks") == 0) {
            double r,g,b;
            token = strtok(NULL, " ");
            r = stod(token);
            token = strtok(NULL, " ");
            g = stod(token);
            token = strtok(NULL, " ");
            b = stod(token);

            material.setSpecular(r, g, b);
            materials.push_back(material);
        }
        else {
            // do nothing
        }
    }
}

double* Scene::getColor(double pos) {
    double* color = new double[3];
    if(pos == -1) {
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
    }
    else {
        double* curr = colors[pos];
        color[0] = 255 * curr[0];
        color[1] = 255 * curr[1];
        color[2] = 255 * curr[2];
        // cout << "Color: " << color[0] << ", " << color[1] << ", " << color[2] << endl;
    }

    for(int i = 0; i < 3; i++) {
        if(color[i] > 255) {
            color[i] = 255;
        }
        if(color[i] < 0) {
            color[i] = 0;
        }
    }

    return color;
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