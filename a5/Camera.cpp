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
// #include <algorithm>
#include <math.h>
//using std::max;
// using std::abs;
// using std::pow;

Camera::Camera() {
}

Camera::Camera(Linear* inc_l) {
    l = inc_l;
    numModels = 0;
}

Camera::~Camera() { }

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
double* Camera::throwRay(int y, int x) {
    intersects.clear();
    intersects.reserve(3000);

    double* none = new double[3];
    none[0] = -1; none[1] = -1; none[2] = -1;
    double px = x / (res[0] - 1) * (right - left) + left;
    double py = y / (res[1] - 1) * (top - btm) + btm;

    double* zAxis = l->scalar(wV, -d, 3);
    double* xAxis = l->scalar(uV, px, 3);
    double* yAxis = l->scalar(vV, py, 3);

    double* temp  = l->add(eye, zAxis, 3);
    double* temp2 = l->add(xAxis, yAxis, 3);
    double* pixpt = l->add(temp, temp2, 3);
    double* ray = l->subtract(pixpt, eye, 3);
    l->makeUnit(ray, 3);

    double* distance = calcSphereIntersect(ray, pixpt, 2, -1);
    if(distance[0] == -1) {
        distance = calcIntersect(ray);
    }

    delete [] temp;
    delete [] temp2;
    delete [] pixpt;
    delete [] zAxis;
    delete [] xAxis;
    delete [] yAxis;
    delete [] ray;

    if(distance[0] == -1.0)
        return none;
    else
        return distance;
}

// Cramer's formula on all the faces
double* Camera::calcIntersect(double* ray) {
    // // store the distances
    vector<Face*> faces;
    double distance;
    vector<double> distVect;
    vector<double> facePos;
    double* none = new double[3];
    none[0] = -1; none[1] = -1; none[2] = -1;
    
    for(unsigned int i = 0; i < models.size(); i++) {
        distance = -1;
        faces = models[i]->getFaces();

        if(distance == -1)
        for(unsigned int j = 0; j < faces.size(); j++) {
            distance = cramers2(faces[j], ray);
            if(distance != -1) {
                if(distance - d > 0){
                    distVect.push_back(distance - d);
                    facePos.push_back((int) i);
                }
            }
        }
    }

    if(distVect.size() == 0)
        return none;

    double small = 9999999;
    int pos = -1;
    for(unsigned int i = 0; i < distVect.size(); i++) {
        double curr = distVect[i];
        if(curr < small){
            small = curr;
            pos = i;
        }
    }

    double* color = calcFaceColor(pos);
    return color;
}

double* Camera::calcFaceColor(int position) {
    double* result = new double[3];
    result[0] = 0; result[1] = 0; result[2] = 0;

    double* color = new double[3];
    color[0] = .2; color[1] = .5; color[2] = .2;

    double ambientReflect[3];
    ambientReflect[0] = ambient[0] * color[0];
    ambientReflect[1] = ambient[1] * color[1];
    ambientReflect[2] = ambient[2] * color[2];

    Point* hit = intersects[position];
    double* intersect = new double[3];
    intersect[0] = hit->getX();
    intersect[1] = hit->getY();
    intersect[2] = hit->getZ();

    Point* normalp = models[0]->normals[position];
    double* normal = new double[3];
    normal[0] = normalp->getX();
    normal[1] = normalp->getY();
    normal[2] = normalp->getZ();

    double* diffuseReflect = diffuseReflection(normal, intersect);
    double hack[3];
    hack[0] = -1; hack[1] = -1; hack[2] = -1;
    double* specularReflect = specularReflection(normal, intersect, &*hack);
    // double specularReflect[3];
    // specularReflect[0] = 0; specularReflect[1] = 0; specularReflect[2] = 0;

    result[0] = (ambientReflect[0] + diffuseReflect[0] + specularReflect[0]) * color[0];
    result[1] = (ambientReflect[1] + diffuseReflect[1] + specularReflect[1]) * color[1];
    result[2] = (ambientReflect[2] + diffuseReflect[2] + specularReflect[2]) * color[2];

    for(int i = 0; i < 3; i++) {
        if(result[i] > 1) {
            result[i] = 1;
        }
        if(result[i] < 0) {
            result[i] = 0;
        }
    }

    return result;
}

double* Camera::calcSphereIntersect(double* ray, double* pixpt, double depth, double lastPos) {
    vector<double> distVect;
    vector<int> spherePos;
    vector<double*> qPoints;
    // I have to do this because my architecture is a mess and I procrastinated too long to refactor
    Point eyePoint = Point(pixpt[0], pixpt[1], pixpt[2]);
    double* qPoint;
    double* none = new double[3];
    none[0] = -1; none[1] = -1; none[2] = -1;

    for(unsigned int i = 0; i < spheres.size(); i++) {
        Sphere* sphere = spheres[i];

        double* tV = sphere->getLoc().subtract(eyePoint);
        double v = l->dot(&*tV, &*ray, 3);

        double rs = pow(sphere->getRadius(), 2);
        double cs = l->dot(&*tV, &*tV, 3);
        double vs = pow(v, 2);

        double ds = rs - (cs - vs);

        // Calculate distance of intersection
        if(ds > 0){
            double* temp = l->scalar(&*ray, (v - sqrt(ds)), 3);
            qPoint = l->add(&*pixpt, temp, 3);

            double* qVect = l->subtract(qPoint, pixpt, 3);
            double distance = l->vLength(qVect, 3);

            if(distance > 0) {
                distVect.push_back(distance);
                spherePos.push_back((int)i);
                qPoints.push_back(qPoint);
            } 

            delete [] temp;
            delete [] qVect;
        }
    }

    if(distVect.size() == 0){
        return none;
    }
    
    double small = 9999999;
    int pos = -1;
    for(unsigned int i = 0; i < distVect.size(); i++) {
        double curr = distVect[i];
        if(curr < small){
            small = curr;
            pos = i;
        }
    } 

    double* color = calcSphereColor(spherePos[pos], qPoints[pos], pixpt, depth, ray, lastPos);
    return color;
}

double* Camera::diffuseReflection(double* normal, double* intersect) {
    double* result = new double[3];
    result[0] = 0; result[1] = 0; result[2] = 0;

    // cout << "Diffuse Normal: " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
    // cout << "Intersect: " << intersect[0] << ", " << intersect[1] << ", " << intersect[2] << endl;

    for(unsigned int i = 0; i < lights.size(); i++) {
        double* toLight = new double[3];
        toLight[0] = lights[i]->getX() - intersect[0];
        toLight[1] = lights[i]->getY() - intersect[1];
        toLight[2] = lights[i]->getZ() - intersect[2];
        l->makeUnit(&(*toLight), 3);

        double theta = l->dot(normal, toLight, 3);

        if(theta > 0.0) {
            result[0] += diffuse[0] * lights[i]->getR() * theta;
            result[1] += diffuse[1] * lights[i]->getG() * theta;
            result[2] += diffuse[2] * lights[i]->getB() * theta;
        } 
        delete [] toLight;
    }

    for(int i = 0; i < 3; i++) {
        if(result[i] > 1) {
            result[i] = 1;
        }
        if(result[i] < 0) {
            result[i] = 0;
        }
    }
    return result;
}

double* Camera::specularReflection(double* normal, double* intersect, double* pixpt) {
    double* result = new double [3];
    result[0] = 0; result[1] = 0; result[2] = 0;

    if(pixpt[0] == -1) {
        pixpt[0] = eye[0];
        pixpt[1] = eye[1];
        pixpt[2] = eye[2];
    }

    double* c = l->subtract(&(*pixpt), &(*intersect), 3);
    l->makeUnit(c, 3);
    double phongConst = 16;

    for(unsigned int i = 0; i < lights.size(); i++) {
        double* toLight = new double[3];

        toLight[0] = lights[i]->getX() - intersect[0];
        toLight[1] = lights[i]->getY() - intersect[1];
        toLight[2] = lights[i]->getZ() - intersect[2];
        l->makeUnit(&(*toLight), 3);

        double nDotL = l->dot(&(*normal), &(*toLight) , 3);
        if(nDotL > 0) {
            double* temp = l->scalar(&(*normal), 2, 3);
            double* lProjN = l->scalar(&(*temp), nDotL, 3);
            delete [] temp;

            double* t = l->subtract(&(*lProjN), &(*toLight), 3);
            l->makeUnit(&(*t), 3);

            double cDotR = l->dot(&(*c), &(*t), 3);
            double phong = pow(cDotR, phongConst);

            if(phong < 0) {
                phong = 0;
            }

            result[0] += specular[0] * lights[i]->getR() * phong;
            result[1] += specular[1] * lights[i]->getG() * phong;
            result[2] += specular[2] * lights[i]->getB() * phong;

            delete [] t;
            delete [] lProjN;
        }

        delete [] toLight;
    }

    for(int i = 0; i < 3; i++) {
        if(result[i] > 1) {
            result[i] = 1;
        }
        if(result[i] < 0) {
            result[i] = 0;
        }
    }

    return result;
}

// Returns a [3] array of RBG values
double* Camera::calcSphereColor(int position, double* intersect, double* pixpt, double depth, double* ray, double lastPos) {
    Sphere sphere = *spheres[position];
    double* result = new double [3];
    result[0] = 0; result[1] = 0; result[2] = 0;

    if(lastPos == position) return result;

    //Calculate ambient reflection
    double ambientReflect[3];
    ambientReflect[0] = ambient[0] * sphere.getR();
    ambientReflect[1] = ambient[1] * sphere.getG();
    ambientReflect[2] = ambient[2] * sphere.getB();

    //Calculate diffuse reflection
    double* normal = new double[3];
    normal[0] = intersect[0] - sphere.getX();
    normal[1] = intersect[1] - sphere.getY();
    normal[2] = intersect[2] - sphere.getZ();
    l->makeUnit(&(*normal), 3);

    double* diffuseReflect = diffuseReflection(normal, intersect);
    double* specularReflect = specularReflection(normal, intersect, pixpt);

    result[0] = (ambientReflect[0] + diffuseReflect[0] + specularReflect[0]) * sphere.getR();
    result[1] = (ambientReflect[1] + diffuseReflect[1] + specularReflect[1]) * sphere.getG();
    result[2] = (ambientReflect[2] + diffuseReflect[2] + specularReflect[2]) * sphere.getB();

    for(int i = 0; i < 3; i++) {
        if(result[i] > 1) {
            result[i] = 1;
        }
        if(result[i] < 0) {
            result[i] = 0;
        }
    }

    delete [] diffuseReflect;
    delete [] specularReflect;

    if(depth > 0) {
        double* uInv = l->scalar(&*ray, -1, 3);
        double* refRTemp = l->scalar(&*normal,  2 * (l->dot(uInv, normal, 3)), 3);
        double* refR = l->subtract(&*refRTemp, uInv, 3);
        l->makeUnit(refR, 3);

        double* accum = calcSphereIntersect(refR, intersect, depth - 1, position);

        if(accum[0] > 0.0 && accum[1] > 0.0 && accum[2] > 0.0)
            for(int i = 0; i < 3; i++) {
                result[i] += accum[i] * (1 * depth);
            }

        delete [] accum;
        delete [] uInv;
        delete [] refRTemp;
        delete [] refR;
    }

    delete [] normal;

    return result;
}

// A la Stack Overflow: http://stackoverflow.com/questions/2411392/double-epsilon-for-equality-greater-than-less-than-less-than-or-equal-to-gre
bool Camera::equals(double x, double y) {
    double temp = 0;
    if(abs(x) > abs(y)) temp = abs(x);
    else temp = abs(y);
    double epsilon = temp * 1E-15;
    return abs(x - y) <= epsilon;
}

double Camera::cramers2(Face* face, double* dv) {
    vector<Point*> trianglePoints = face->getPoints();
    Point* triangleA = trianglePoints[0];
    Point* triangleB = trianglePoints[1];
    Point* triangleC = trianglePoints[2];

    double ax = triangleA->getX();
    double ay = triangleA->getY();
    double az = triangleA->getZ();

    double bx = triangleB->getX();
    double by = triangleB->getY();
    double bz = triangleB->getZ();

    double cx = triangleC->getX();
    double cy = triangleC->getY();
    double cz = triangleC->getZ();

    double lx = eye[0];
    double ly = eye[1];
    double lz = eye[2];

    double dx = dv[0];
    double dy = dv[1];
    double dz = dv[2];

    double denominator = ((az - cz) * dy - (ay - cy) * dz) * (ax - bx) - ((az - cz) * dx - (ax - cx) * dz) * (ay - by)+ ((ay - cy) * dx - (ax - cx) * dy) * (az - bz);

    if(denominator == 0)
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

    double* bMinusA = triangleB->subtract(*triangleA);
    double* cMinusA = triangleC->subtract(*triangleA);

    bMinusA[0] = bMinusA[0] * beta;
    cMinusA[0] = cMinusA[0] * gamma;

    bMinusA[1] = bMinusA[1] * beta;
    cMinusA[1] = cMinusA[1] * gamma;

    bMinusA[2] = bMinusA[2] * beta;
    cMinusA[2] = cMinusA[2] * gamma;

    Point* hit = new Point( triangleA->getX() + bMinusA[0] + cMinusA[0],
                            triangleA->getY() + bMinusA[1] + cMinusA[1],
                            triangleA->getZ() + bMinusA[2] + cMinusA[2]);

    intersects.push_back(hit);

    delete [] bMinusA;
    delete [] cMinusA;

    return t;
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
