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
using std::pow;

Camera::Camera() {
}

Camera::Camera(Linear* inc_l) {
    l = inc_l;
    numModels = 0;
}

Camera::~Camera() { 
    // delete [] uV;
    // delete [] vV;
    // delete [] wV;
}

void Camera::calcBasis() {
    wV = l->subtract(eye, look, 3);
    l->makeUnit(wV, 3);

    uV = l->cross3(up, wV);
    if(uV[0] == 0 && uV[1] == 0 && uV[2] == 0)
        uV[0] = 1;
    l->makeUnit(uV, 3);

    vV = l->cross3(wV, uV);
}

// Should throw 0 or something that isn't -1 because -1 causes an error as defined by me
// Returns -1 if no intersection, and distance of the intersection if the intersection exists
double Camera::throwRay(int x, int y) {
    double px = x / (res[0] - 1) * (right - left) + left;
    double py = y / (res[1] - 1) * (top - btm) + btm;

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

    // cout << "zAxis: [" << zAxis[0] << ", " << zAxis[1] << ", " << zAxis[2] << "]" << endl;
    // cout << "xAxis: [" << xAxis[0] << ", " << xAxis[1] << ", " << xAxis[2] << "]" << endl;
    // cout << "yAxis: [" << yAxis[0] << ", " << yAxis[1] << ", " << yAxis[2] << "]" << endl;

    // cout << "temp: [" << temp[0] << ", " << temp[1] << ", " << temp[2] << "]" << endl;
    // cout << "temp2: [" << temp2[0] << ", " << temp2[1] << ", " << temp2[2] << "]" << endl;

    // cout << "Ray passed in: [" << ray[0] << ", " << ray[1] << ", " << ray[2] << "]" << endl;

    l->makeUnit(ray, 3);

    double distance = calcSphereIntersect(ray);
    if(distance == -1) {
        distance = calcIntersect(ray);
    }
    else {
        // for debugging
        cout << "Sphere intersect" << endl;
    }

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
    vector<Face> faces;
    double distance;
    vector<double> distVect;

    for(unsigned int i = 0; i < models.size(); i++) {
        distance = -1;
        faces = models[i]->getFaces();

        if(distance == -1)
        for(unsigned int j = 0; j < faces.size(); j++) {
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

double Camera::calcSphereIntersect(double* ray) {
    vector<double> distVect;
    vector<int> spherePos;
    // I have to do this because my architecture is a mess and I procrastinated too long to refactor
    Point eyePoint = Point(eye[0], eye[1], eye[2]);
    // cout << "Ray: [" << ray[0] << ", " << ray[1] << ", " << ray[2] << "]" << endl; 


    for(unsigned int i = 0; i < spheres.size(); i++) {
        Sphere* sphere = spheres[i];

        double* cVect = sphere->getLoc().subtract(eyePoint);
        double c = l->vLength(cVect, 3);

        // cout << "C: [" << cVect[0] << ", " << cVect[1] << ", " << cVect[2] << "]" << endl;
        // cout << "c: " << c << endl;

        //Project c onto ray
        // v = (c * ray / ray * ray) * ray
        double top = l->dot(cVect, &*ray, 3);
        // double bot = l->dot(&*ray, &*ray, 3);
        double bot = l->vLength(&*ray, 3);
        double* vVect = l->scalar(&*ray, (top / bot), 3);
        double v = l->vLength(vVect, 3);

        double rs = pow(sphere->getRadius(), 2);
        double cs = pow(c, 2);
        double vs = pow(v, 2);

        // cout << "r: " << sphere->getRadius() << endl;
        // cout << "rs: " << rs << endl;
        // cout << "cs: " << cs << endl;
        // cout << "vs: " << vs << endl;

        double ds = rs - (cs - vs);
        cout << "d squared: " << ds << endl;

        // Calculate distance of intersection
        if(ds >= 0){
            // cout << "D doesn't suck" << endl;
            //Q = E + (v-d)R
            double* temp = l->scalar(&*ray, (v - d), 3);
            double* qPoint = l->add(eye, temp, 3);
            double* qVect = l->subtract(qPoint, eye, 3);
            double distance = l->vLength(qVect, 3);

            if(distance > 0) {
                distVect.push_back(distance);
                spherePos.push_back((int)i);
            } 

            delete [] temp;
            delete [] qVect;
            delete [] qPoint;
        }

        delete [] vVect;
        delete [] cVect;
    }

    if(distVect.size() == 0)
        return -1;
    
    double small = 9999999;
    int pos;
    for(unsigned int i = 0; i < distVect.size(); i++) {
        double curr = distVect[i];
        if(curr < small){
            small = curr;
            pos = i;
        }
    } 
    // pos should be the position in the sphere vector of the sphere. Do color stuff!
    return small;
}

double Camera::cramers2(Face* face, double* dv) {
    vector<Point> trianglePoints = face->getPoints();
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
            // This is pretty hacky, but it returns the file when it's done reading the last value so that the scene
            //  gets to start where it should
            return file;
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