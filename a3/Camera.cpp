#include "Camera.h"
#include "Linear.h"
#include "Model.h"
#include "Face.h"
#include "Point.h"

using std::ifstream;
using std::cout;
using std::endl;
#include <string>
using std::string;
using std::stod;
#include <cstring>

Camera::Camera(string filename, Linear* inc_l, Model* inc_models, int inc_numModels) {
    l = inc_l;
    models = inc_models;
    numModels = inc_numModels;

    char cFilename [256];
    std::strcpy(cFilename, filename.c_str());

    ifstream file(cFilename);
    read(file);
}

Camera::~Camera() { }

// Should throw 0 or something that isn't -1 because -1 causes an error as defined by me
// This is leaking like a madman, write a class for vectors ya dingus
// Returns -1 if no intersection, and distance of the intersection if the intersection exists
double Camera::throwRay(int x, int y) {

    double px = (x / (res[0] - 1)) * (right - left) + left;
    double py = (y / (res[1] - 1)) * (top - btm) + btm;
    
    // WV
    double* zAxis = l->subtract(eye, look, 3);
    l->makeUnit(zAxis, 3);
    // UV
    double* xAxis = l->cross3(zAxis, up);
    l->makeUnit(xAxis, 3);
    // VV
    double* yAxis = l->cross3(zAxis, xAxis); // implicitly unit length

    zAxis = l->scalar(zAxis, d, 3);
    xAxis = l->scalar(xAxis, px, 3);
    yAxis = l->scalar(yAxis, py, 3);

    double* temp  = l->add(eye, zAxis, 3);
    double* temp2 = l->add(xAxis, yAxis, 3);
    double* pixpt = l->add(temp, temp2, 3);
    double* ray = l->subtract(pixpt, eye, 3);

    cout << "Ray calculated, calculating intersect . . ." << endl;

    double distance = calcIntersect(ray);

    cout << "Distance calculated, clearing data . . ." << endl;

    // distance from the image plane to the intersection rather than the image plane
    // distance -= 2

    delete [] temp;
    delete [] temp2;
    delete [] pixpt;
    delete [] zAxis;
    delete [] xAxis;
    delete [] yAxis;
    delete [] ray;

    if(distance == -1.0)
        return -1.0;
    else
        return distance;
}

// Cramer's formula on all the faces
double Camera::calcIntersect(double* ray) {
    //incoming barrage of obnoxious variable names
    Face* faces;
    int numFaces = -1;
    double distance = -1;   
    for(int i = 0; i < numModels; i++) {
        numFaces = models[i].getNumFaces();
        faces = models[i].getFaces();
        cout << "Faces: " << numFaces << endl;

        for(int j = 0; j < numFaces; j++) {
            cout << "Checking intersection with face " << j << ". . ." << endl;
            distance = cramers(&faces[j], ray);
            if(distance != -1)
                return distance;
        }
    }
    return -1;
}

// -1 if no intersect
double Camera::cramers(Face* face, double* ray) {
    cout << "Getting points . . ." << endl;
    Point* trianglePoints = face->getPoints();
    cout << "Points received!" << endl;
    Point triangleA = trianglePoints[0];
    Point triangleB = trianglePoints[1];
    Point triangleC = trianglePoints[2];
    cout << "Points unrolled" << endl;

    // calc beta
    // make sure beta is less than equal to 1, and greater than equal to zero
    // abc def ghi jkl z for beta
    double a = triangleA.getZ() - triangleC.getZ();
    double b = triangleA.getY() - triangleC.getY();
    double c = triangleA.getX() - triangleC.getX();

    double d = triangleA.getX() - eye[0];
    double e = triangleA.getY() - eye[1];
    double f = triangleA.getZ() - eye[2];

    double g = triangleA.getX() - triangleB.getX();
    double h = triangleA.getY() - triangleB.getY();
    double i = triangleA.getZ() - triangleB.getZ();

    double j = (a * ray[1]) - (b * ray[2]);
    double k = (a * ray[0]) - (c * ray[3]);
    double l = (b * ray[0]) - (c * ray[1]);

    double z = (j * g) - (k * h) + (l * i);

    double beta = ((j * d) - (k * e) + (l * f)) / z;

    cout << "Beta calculated: " << beta << endl;

    if(beta < 0 || beta > 1)
        return -1;
    // calc gamma and check the values don't suck
    // mno
    double m = (f * ray[1]) - (e * ray[2]);
    double n = (f * ray[0]) - (d * ray[2]);
    double o = (e * ray[0]) - (d * ray[1]);

    double gamma = ((m * g) - (n * h) + (o * i)) / z;

    cout << "Gamma calculated: " << gamma << endl;
    if(gamma < 0 || gamma > 1)
        return -1.0;

    if(beta + gamma > 1)
        return -1.0;

    // then t
    // p q r
    double t = ((((e * a) - (b * f)) * g) - (((d * a) - (c * f)) * h) 
                + (((f * b) - (c * e)) * l) ) / z;

    cout << "t calculated: " << t << endl;

    if(t <= 0) {
        return -1.0;
    }

    return t;
}

void Camera::read(ifstream& file) {
    if (file.fail()) {
        file.clear();
        return;
    }
    string curr;
    char* token;
    char* currChars;

    do {
        getline(file, curr);
        currChars = const_cast<char*>(curr.c_str());
        token = strtok(currChars, " ");

        // return if endline is the last character in the file
        if(strcmp(currChars, "") == 0)
            return;

        if (strcmp(token, "eye") == 0) {
            for(int i = 0; i < 3; i++) {
                token = strtok(NULL, " ");
                eye[i] = stod(token);
            }
        }
        else if (strcmp(token, "look") == 0) {
            for(int i = 0; i < 3; i++) {
                token = strtok(NULL, " ");
                look[i] = stod(token);
            }
        }
        else if (strcmp(token, "up") == 0) {
            for(int i = 0; i < 3; i++) {
                token = strtok(NULL, " ");
                up[i] = stod(token);
            }
        }
        else if (strcmp(token, "d") == 0) {
            token = strtok(NULL, " ");
            d = stod(token);
        }
        else if (strcmp(token, "bounds") == 0) {
            token = strtok(NULL, " ");
            left = stod(token);

            token = strtok(NULL, " ");
            btm = stod(token);

            token = strtok(NULL, " ");
            right = stod(token);

            token = strtok(NULL, " ");
            top = stod(token);
            // for(int i = 0; i < 4; i++) {
            //     token = strtok(NULL, " ");
            //     bounds[i] = stod(token);
            // }
        }
        else if (strcmp(token, "res") == 0) {
            for(int i = 0; i < 2; i++) {
                token = strtok(NULL, " ");
                res[i] = stod(token);
            }
        }
        else {
            // skip empty / nonsense lines
        }
    } while(!file.eof());

    file.close();
}

void Camera::printCamera(){
    cout << "Eye: ";
    for(int i = 0; i < 3; i++) {
        cout << eye[i] << ", ";
    }
    cout << endl << "Look: ";
    for(int i = 0; i < 3; i++) {
        cout << look[i] << ", ";
    }
    cout << endl << "Up: ";
    for(int i = 0; i < 3; i++) {
        cout << up[i] << ", ";
    } 
    cout << endl << "d: " << d << endl;
    cout << "Bounds: " << left << ", " << btm << ", " << right << ", " << top;
    cout << endl << "Res: " << res[0] << ", " << res[1] << endl;
}