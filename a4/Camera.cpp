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
#include <vector>
using std::vector;
#include <algorithm>
using std::max;
using std::abs;

Camera::Camera() {
}

Camera::Camera(Linear* inc_l, Model* inc_models, int inc_numModels) {
    l = inc_l;
    models = inc_models;
    numModels = inc_numModels;
}

Camera::Camera(string filename, Linear* inc_l, Model* inc_models, int inc_numModels) {
    l = inc_l;
    models = inc_models;
    numModels = inc_numModels;

    char cFilename [256];
    std::strcpy(cFilename, filename.c_str());

    ifstream file(cFilename);
    read(file);
    calcBasis();
}

Camera::~Camera() { 
    delete [] uV;
    delete [] vV;
    delete [] wV;
}

void Camera::calcBasis() {
    wV = l->subtract(eye, look, 3);
    l->makeUnit(wV, 3);

    uV = l->cross3(up, wV);
    l->makeUnit(uV, 3);

    vV = l->cross3(wV, uV);
}

// Should throw 0 or something that isn't -1 because -1 causes an error as defined by me
// This is leaking like a madman, write a class for vectors ya dingus
// Returns -1 if no intersection, and distance of the intersection if the intersection exists
double Camera::throwRay(int x, int y) {
    double px = x / (res[0] - 1) * (right - left) + left;
    double py = y / (res[1] - 1) * (top - btm) + btm;
    // cout << "px: " << px << ", py: " << py << endl;

    // wV
    double* zAxis = l->scalar(wV, -d, 3);
    // uV
    double* xAxis = l->scalar(uV, px, 3);
    // vV
    double* yAxis = l->scalar(vV, py, 3);

    double* temp  = l->add(eye, zAxis, 3);
    double* temp2 = l->add(xAxis, yAxis, 3);
    double* pixpt = l->add(temp, temp2, 3);
    double* ray = l->subtract(pixpt, eye, 3);

    // cout << "Ray calculated: [" << ray[0] << ", " << ray[1] << ", " << ray[2] << "]" << endl;
    l->makeUnit(ray, 3);
    // cout << "Ray calculated: [" << ray[0] << ", " << ray[1] << ", " << ray[2] << "]" << endl;
    double distance = calcIntersect(ray);

    // cout << "Distance calculated: " << distance << endl;

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
    // store the distances
    Face* faces;
    int numFaces = -1;
    double distance = -1;
    vector<double> distVect;
    for(int i = 0; i < numModels; i++) {
        numFaces = models[i].getNumFaces();
        faces = models[i].getFaces();
        for(int j = 0; j < numFaces; j++) {
            // cout << "Checking intersection with face " << j << ". . ." << endl;
            distance = cramers2(&faces[j], ray);
            if(distance != -1) {
                if(distance - d > 0)
                    distVect.push_back(distance - d);
            }
        }
    }

    if(distVect.size() == 0)
        return -1;

    double small = 9999999;
    for(unsigned int i = 0; i < distVect.size(); i++) {
        double curr = distVect[i];
        if(curr < small)
            small = curr;
    }
    return small;
}

// -1 if no intersect
double Camera::cramers(Face* face, double* ray) {
    Point* trianglePoints = face->getPoints();
    Point triangleA = trianglePoints[0];
    Point triangleB = trianglePoints[1];
    Point triangleC = trianglePoints[2];

    // calc beta
    // make sure beta is less than equal to 1, and greater than equal to zero
    // abc def ghi jkl z for betaLeft handed 
    double a = triangleA.getZ() - triangleC.getZ();
    double b = triangleA.getY() - triangleC.getY();
    double c = triangleA.getX() - triangleC.getX();

    double d2 = triangleA.getX() - eye[0];
    double e = triangleA.getY() - eye[1];
    double f = triangleA.getZ() - eye[2];

    double g = triangleA.getX() - triangleB.getX();
    double h = triangleA.getY() - triangleB.getY();
    double i = triangleA.getZ() - triangleB.getZ();

    double j = (a * ray[1]) - (b * ray[2]);
    double k = (a * ray[0]) - (c * ray[2]);
    double l = (b * ray[0]) - (c * ray[1]);

    double z = (j * g) - (k * h) + (l * i);

    if(equals(z, 0.0)) {
        return -1;
    }

    double beta = ((j * d2) - (k * e) + (l * f)) / z;

    // cout << "Beta calculated: " << beta << endl;

    if(beta < 0 || beta > 1)
        return -1;
    // calc gamma and check the values don't suck
    // mno
    double m = (f * ray[1]) - (e * ray[2]);
    double n = (f * ray[0]) - (d2 * ray[2]);
    double o = (e * ray[0]) - (d2 * ray[1]);

    double gamma = ((m * g) - (n * h) + (o * i)) / z;

    // cout << "Gamma calculated: " << gamma << endl;
    if(gamma < 0 || gamma > 1)
        return -1.0;

    if(beta + gamma > 1)
        return -1.0; 

    // then t
    // p q r
    double p = ((e * a) - (b * f)) * g;
    double q = ((d2 * a) - (c * f)) * h;
    double r = ((f * b) - (c * e)) * i;
    double t = (p - q + r) / z;

    if(t > 8000) {
        cout << "t: " << t << ", z: " << z << endl;
        cout << "beta: " << beta << ", gamma: " << gamma << endl;
        cout << "p: " << p << endl;
        cout << "e: " << e << ", a: " << a << ", (e*a): " << e*a << ", g: " << g << endl;
        cout << "q: " << q << endl;
        cout << "r: " << r << endl << endl;
    }

    // cout << "t calculated: " << t << endl;

    if(t < 0) {
        return -1.0;
    }

    return t;
}

double Camera::cramers2(Face* face, double* dv) {
    Point* trianglePoints = face->getPoints();
    Point triangleA = trianglePoints[0];
    Point triangleB = trianglePoints[1];
    Point triangleC = trianglePoints[2];

    double ax = triangleA.getX();
    double ay = triangleA.getY();
    double az = triangleA.getZ();

    double bx = triangleB.getX();
    double by = triangleB.getY();
    double bz = triangleB.getZ();

    double cx = triangleC.getX();
    double cy = triangleC.getY();
    double cz = triangleC.getZ();

    double lx = eye[0];
    double ly = eye[1];
    double lz = eye[2];

    double dx = dv[0];
    double dy = dv[1];
    double dz = dv[2];

    double denominator = ((az - cz) * dy - (ay - cy) * dz) * (ax - bx) - ((az - cz) * dx - (ax - cx) * dz) * (ay - by)
                            + ((ay - cy) * dx - (ax - cx) * dy) * (az - bz);

    if(equals(denominator, 0.0) || denominator == 0)
        return -1.0;

    double beta = ( ((az - cz) * dy - (ay - cy) * dz) * (ax - lx) - ((az - cz) * dx - (ax - cx) * dz) * (ay - ly)
                    + ((ay - cy) * dx - (ax - cx) * dy) * (az - lz) ) / denominator;

    if(beta < 0 || beta > 1)
        return -1;

    double gamma = ( ((az - lz) * dy - (ay - ly) * dz) * (ax - bx) - ((az - lz) * dx - (ax - lx) * dz) * (ay - by)
                    + ((ay - ly) * dx - (ax - lx) * dy) * (az - bz) ) / denominator;

    if(gamma < 0 || gamma > 1)
        return -1.0;

    if(beta + gamma > 1)
        return -1.0; 
    

    double t = ( ((ay - ly) * (az - cz) - (ay - cy) * (az - lz)) * (ax - bx) - ((ax - lx) * (az - cz) - (ax - cx) * (az - lz)) * (ay - by)
                + ((ax - lx) * (ay - cy) - (ax - cx) * (ay - ly)) * (az - bz) ) / denominator;

    if(t < 0)
        return -1.0;


    return t;
}

// A la Stack Overflow: http://stackoverflow.com/questions/2411392/double-epsilon-for-equality-greater-than-less-than-less-than-or-equal-to-gre
bool Camera::equals(double x, double y) {
    double epsilon = max(abs(x), abs(y)) * 1E-15;
    return abs(x - y) <= epsilon;
}

ifstream& Camera::read(ifstream& file) {
    if (file.fail()) {
        file.clear();
        return file;
    }
    string curr;
    char* token;
    char* currChars;
    bool done = false;

    while(!done) {
        getline(file, curr);
        currChars = const_cast<char*>(curr.c_str());
        token = strtok(currChars, " ");

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
            btm = stod(token);
            
            token = strtok(NULL, " ");
            left = stod(token);

            token = strtok(NULL, " ");
            top = stod(token);

            token = strtok(NULL, " ");
            right = stod(token);
        }
        else if (strcmp(token, "res") == 0) {
            for(int i = 0; i < 2; i++) {
                token = strtok(NULL, " ");
                res[i] = stod(token);
            }
        }
        else {
            done = true;
        }
    }

    return file;
    // The file structure changed so this is only the top of the file now
    // file.close();
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