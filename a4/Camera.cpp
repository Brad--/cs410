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
//#include <algorithm>
#include <math.h>
//using std::max;
//using std::abs;
//using std::pow;

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
double* Camera::throwRay(int x, int y) {
    intersects.clear();
    intersects.reserve(3000);

    double* none = new double[3];
    none[0] = -1; none[1] = -1; none[2] = -1;
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

    double* distance = calcSphereIntersect(ray);
    if(distance[0] == -1) {
        distance = calcIntersect(ray);
    }
    // cout << "Distance calculated: " << distance << endl;

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
    // store the distances
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
            // faces[j]->print();
            // cout << j << endl;
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
    // double ambientReflect[3] = {0, 0, 0};

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

    double* ctemp = new double [3];
    ctemp[0] = 0; ctemp[1] = 0; ctemp[2] = 0;
    double* diffuseReflect = diffuseReflection(normal, ctemp, intersect);
    // double diffuseReflect[3] = {0,0,0};

    double* specularReflect = specularReflection(normal, intersect);
    // double specularReflect[3] = {0,0,0};

    // cout << "DifRef: " << diffuseReflect[0] << ", " << diffuseReflect[1] << ", " << diffuseReflect[2] << endl;
    // cout << "SpecRef: " << specularReflect[0] << ", " << specularReflect[1] << ", " << specularReflect[2] << endl;

    result[0] = (ambientReflect[0] + diffuseReflect[0] + specularReflect[0]);
    result[1] = (ambientReflect[1] + diffuseReflect[1] + specularReflect[1]);
    result[2] = (ambientReflect[2] + diffuseReflect[2] + specularReflect[2]);

    for(int i = 0; i < 3; i++) {
        if(result[i] > 1) {
            result[i] = 1;
        }
        if(result[i] < 0) {
            result[i] = 0;
        }
        result[i] *= 255;
    }

    return result;
}

double* Camera::calcSphereIntersect(double* ray) {
    vector<double> distVect;
    vector<int> spherePos;
    // I have to do this because my architecture is a mess and I procrastinated too long to refactor
    Point eyePoint = Point(eye[0], eye[1], eye[2]);
    double* qPoint;
    double* none = new double[3];
    none[0] = -1; none[1] = -1; none[2] = -1;
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
        // cout << "d squared: " << ds << endl;

        // Calculate distance of intersection
        if(ds >= 0){
            // cout << "D doesn't suck" << endl;
            //Q = E + (v-d)R
            double* temp = l->scalar(&*ray, (v - d), 3);
            qPoint = l->add(eye, temp, 3);
            double* qVect = l->subtract(qPoint, eye, 3);
            double distance = l->vLength(qVect, 3);

            if(distance > 0) {
                distVect.push_back(distance);
                spherePos.push_back((int)i);
            } 

            delete [] temp;
            delete [] qVect;
            // delete [] qPoint;
        }

        delete [] vVect;
        delete [] cVect;
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
    
    // pos should be the position in the sphere vector of the sphere. Do color stuff!

    double* color = calcSphereColor(pos, qPoint);
    return color;
}

double* Camera::diffuseReflection(double* normal, double* color, double* intersect) {
    double* result = new double[3];
    result[0] = 0; result[1] = 0; result[2] = 0;

    for(unsigned int i = 0; i < lights.size(); i++) {
        // lights[i]->printLight();
        double* toLight = new double[3];
        toLight[0] = lights[i]->getX() - intersect[0];
        toLight[1] = lights[i]->getY() - intersect[1];
        toLight[2] = lights[i]->getZ() - intersect[2];

        // toLight[0] = intersect[0] - lights[i]->getX();
        // toLight[1] = intersect[1] - lights[i]->getY();
        // toLight[2] = intersect[2] - lights[i]->getZ();
        l->makeUnit(&(*toLight), 3);

        double theta = l->dot(normal, toLight, 3);
        // theta = cos(theta);
        if(theta < 0) {
            theta = theta * -1;
        }
        // cout << "Theta: " << theta << endl;

        result[0] += lights[i]->getR() * theta;
        result[1] += lights[i]->getG() * theta;
        result[2] += lights[i]->getB() * theta;

        // result[0] += ambient[0] * lights[i]->getR() * theta;
        // result[1] += ambient[1] * lights[i]->getG() * theta;
        // result[2] += ambient[2] * lights[i]->getB() * theta;

        delete [] toLight;
    }

    for(int i = 0; i < 3; i++) {
        if(result[i] > 1) {
            result[i] = 1;
        }
        if(result[i] < 0) {
            result[i] = 0;
        }
        // result[i] *= 255;
    }
    return result;
}

double* Camera::specularReflection(double* normal, double* intersect) {
    double* result = new double [3];
    result[0] = 0; result[1] = 0; result[2] = 0;
    double* c = l->subtract(&(*eye), &(*intersect), 3);
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
            // N * (L dot N)
            double* lProjN = l->scalar(&(*normal), 2 * nDotL, 3);
            // NL - L
            // double* t = l->scalar(l->subtract(&(*lProjN), &(*toLight), 3), 2, 3);
            double* t = l->subtract(&(*lProjN), &(*toLight), 3);

            // L + 2T
            // double* r = l->add(&(*toLight), l->scalar(&(*t), 2, 3), 3);
            // double* r =l->scalar(&(*lProjN), 1, 3);
            // double* r = l->add(&(*toLight), &(*t), 3)
            double*r = lProjN;
            l->makeUnit(r, 3);

            double cDotR = l->dot(&(*c), &(*r), 3);
            // cout << "v dot R: " << cDotR << endl;
            if(cDotR < 0) {
                cDotR = 0;
            }

            double phong = pow(cDotR, phongConst);
            if(phong < 0) {
                phong = 0;
            }

            // cout << "Phong: " << phong << endl;

            result[0] += lights[i]->getR() * phong;
            result[1] += lights[i]->getG() * phong;
            result[2] += lights[i]->getB() * phong;

            // result[0] += ambient[0] * lights[i]->getR() * phong;
            // result[1] += ambient[1] * lights[i]->getG() * phong;
            // result[2] += ambient[2] * lights[i]->getB() * phong;

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
        // result[i] *= 255;
    }

    return result;
}

// Returns a [3] array of RBG values
double* Camera::calcSphereColor(int position, double* intersect) {
    Sphere sphere = *spheres[position];
    double* result = new double [3];

    // cout <<"Intersect: " << intersect[0] << ", " << intersect[1] << ", " << intersect [2] << endl;

    //Calculate ambient reflection
    double ambientReflect[3];
    ambientReflect[0] = ambient[0] * sphere.getR();
    ambientReflect[1] = ambient[1] * sphere.getG();
    ambientReflect[2] = ambient[2] * sphere.getB();
    // double ambientReflect[3] = {0, 0, 0};

    //Calculate diffuse reflection
    double* normal = new double[3];
    normal[0] = intersect[0] - sphere.getX();
    normal[1] = intersect[1] - sphere.getY();
    normal[2] = intersect[2] - sphere.getZ();

    // normal[0] = sphere.getX() - intersect[0];
    // normal[1] = sphere.getY() - intersect[1];
    // normal[2] = sphere.getZ() - intersect[2];
    l->makeUnit(&(*normal), 3);

    double* ctemp = new double [3];
    ctemp[0] = sphere.getR();
    ctemp[1] = sphere.getG();
    ctemp[2] = sphere.getB();
    double* diffuseReflect = diffuseReflection(normal, ctemp, intersect);
    // double diffuseReflect[3] = {0,0,0};

    double* specularReflect = specularReflection(normal, intersect);
    // double specularReflect[3] = {0,0,0};

    // cout << "DifRef: " << diffuseReflect[0] << ", " << diffuseReflect[1] << ", " << diffuseReflect[2] << endl;
    // cout << "SpecRef: " << specularReflect[0] << ", " << specularReflect[1] << ", " << specularReflect[2] << endl;

    result[0] = (ambientReflect[0] + diffuseReflect[0] + specularReflect[0]);
    result[1] = (ambientReflect[1] + diffuseReflect[1] + specularReflect[1]);
    result[2] = (ambientReflect[2] + diffuseReflect[2] + specularReflect[2]);

    for(int i = 0; i < 3; i++) {
        if(result[i] > 1) {
            result[i] = 1;
        }
        if(result[i] < 0) {
            result[i] = 0;
        }
        result[i] *= 255;
    }

    // delete [] diffuseReflect;
    delete [] normal;
    delete [] ctemp;

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

    if(equals(denominator, 0))
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
