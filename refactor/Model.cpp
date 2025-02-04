#include "Model.h"

using std::ifstream;
using std::cout;
using std::endl;
#include <string>
#include <cstring>
using std::stoi;
using std::stod;
using std::strtok;
using std::strcmp;
using std::string;
#include <math.h>

Model::Model(string filename) {
    init(filename);
}

Model::~Model() {  }

void Model::init(string filename) {
    char cFilename [256];
    std::strcpy(cFilename, filename.c_str());

    ifstream file(cFilename);
    read(file, materials);
}
bool Model::read(ifstream& file, vector<Material> m) {
    if (file.fail()) {
        file.clear();
        return false;
    }
    materials = m;

    Linear l = Linear();

    faces.reserve(2500);
    points.reserve(3000);
    normals.reserve(3000);

    string curr;
    char *token;
    char *currChars;
    int matCount = 0;

    while(!file.eof()) {
        getline(file, curr);
        currChars = const_cast<char*>(curr.c_str());
        token = strtok(currChars, " ");

        // if(token[0] != '#' && curr.find("/") != std::string::npos) {
        if(token != nullptr && token[0] != '#') {
            // Read vertex line
            if(strcmp(token, "v") == 0) {
                token = strtok(NULL, " ");
                double tx = stod(token);
                token = strtok(NULL, " ");
                double ty = stod(token);
                token = strtok(NULL, " ");
                double tz = stod(token);

                Point* tempPoint = new Point(tx, ty, tz);
                points.push_back(tempPoint);
            }
            // Read texture coordinate
            if(strcmp(token, "vt") == 0) {
                // do nothing
            }
            // Read surface normal (Not necessarily unit)
            if(strcmp(token, "vn") == 0) {
                double tx, ty, tz;

                token = strtok(NULL, " ");
                tx = stod(token);
                token = strtok(NULL, " ");
                ty = stod(token);
                token = strtok(NULL, " ");
                tz = stod(token);

                Point* tempPoint = new Point(tx, ty, tz);
                double* tp = new double[3];
                tp[0] = tempPoint->getX();
                tp[1] = tempPoint->getY();
                tp[2] = tempPoint->getZ();
                l.makeUnit(&(*tp), 3);

                tempPoint->setX(tp[0]);
                tempPoint->setY(tp[1]);
                tempPoint->setZ(tp[2]);

                normals.push_back(tempPoint);
            }
            // Read parameter space vertices (idk what that means)
            if(strcmp(token, "vp") == 0) {
                // do nothings
            }
            // Read face element (also don't know this)
            if(strcmp(token, "f") == 0) {
                Face* tempFace = new Face();
                vector<Point*> facePoints;
                int tx, ty, tz;

                token = strtok(NULL, " ");
                tx = stoi(token);
                token = strtok(NULL, " ");
                ty = stoi(token);
                token = strtok(NULL, " ");
                tz = stoi(token);

                facePoints.push_back(points[tx - 1]);
                facePoints.push_back(points[ty - 1]);
                facePoints.push_back(points[tz - 1]);

                // cout << faces.size() << endl;
                // for(int i = 0; i < 3; i++) {
                //     facePoints[0]->print();
                // }

                tempFace->setPoints(facePoints);
                
                faces.push_back(tempFace);
            }
            if(strcmp(token, "usemtl") == 0) {
                if(matCount != 0){
                    materials[matCount].setEnd(faces.size());
                    matCount++;
                }
                int begin = faces.size();
                if(begin > 0)
                    begin += 1;
                // materials[matCount].setBegin(faces.size());
            }
        }
    }

    return true;
}

bool Model::write(string filename, string ext) const {

    return true;
}

Point Model::avgPoint() const {
    double avgX = 0.0;
    double avgY = 0.0;
    double avgZ = 0.0;

    for(unsigned int i = 0; i < points.size(); i++) {
        avgX += points[i]->getX();
        avgY += points[i]->getY();
        avgZ += points[i]->getZ();
    }

    avgX /= numPoints;
    avgY /= numPoints;
    avgZ /= numPoints;

    return Point(avgX, avgY, avgZ);
}

Point Model::stdDev() const {
    Point mean = avgPoint();
    double devX = 0.0;
    double devY = 0.0;
    double devZ = 0.0;

    // Calculate sum of squares
    for(unsigned int i = 0; i < points.size(); i++) {
        devX += pow((points[i]->getX() - mean.getX()), 2);
        devY += pow((points[i]->getY() - mean.getY()), 2);
        devZ += pow((points[i]->getZ() - mean.getZ()), 2);
    }

    // Variance
    devX /= numPoints;
    devY /= numPoints;
    devZ /= numPoints;

    // Standard Deviation
    devX = sqrt(devX);
    devY = sqrt(devY);
    devZ = sqrt(devZ);

    return Point(devX, devY, devZ);
 }

void Model::center() {
    Point meanPoint = avgPoint();
    Point *currPoint;
    for(unsigned int i = 0; i < points.size(); i++) {
        currPoint = points[i];
        currPoint->setX(currPoint->getX() - meanPoint.getX());
        currPoint->setY(currPoint->getY() - meanPoint.getY());
        currPoint->setZ(currPoint->getZ() - meanPoint.getZ());
    }
}

void Model::whiten() {
    Point stdDevPoint = stdDev();
    Point *currPoint;
    for(unsigned int i = 0; i < points.size(); i++) {
        currPoint = points[i];
        currPoint->setX(currPoint->getX() / stdDevPoint.getX());
        currPoint->setY(currPoint->getY() / stdDevPoint.getY());
        currPoint->setZ(currPoint->getZ() / stdDevPoint.getZ());
    }
}

void Model::normalize() {
    center();
    whiten();
}

void Model::translate(double x, double y, double z) {
    for(unsigned int i = 0; i < points.size(); i++) {
        points[i]->translate(x, y, z);
    }
}

void Model::printPoints() {
    double tempX, tempY, tempZ;
    for(unsigned int i = 0; i < points.size(); i++) {
        tempX = points[i]->getX();
        tempY = points[i]->getY();
        tempZ = points[i]->getZ();
        cout << i << ":\t" << tempX << "\t" << tempY << "\t" << tempZ << endl;
    }
}

void Model::printBoundingBox() {
    double maxX = 0.0;
    double maxY = 0.0;
    double maxZ = 0.0;
    double minX = 999999.0;
    double minY = 999999.0;
    double minZ = 999999.0;

    double tempX, tempY, tempZ;
    for(unsigned int i = 0; i < points.size(); i++) {
        tempX = points[i]->getX();
        tempY = points[i]->getY();
        tempZ = points[i]->getZ();

        if(tempX > maxX) 
            maxX = tempX;
        if(tempX < minX)
            minX = tempX;

        if(tempY > maxY) 
            maxY = tempY;
        if(tempY < minY)
            minY = tempY;

        if(tempZ > maxZ) 
            maxZ = tempZ;
        if(tempZ < minZ)
            minZ = tempZ;
    }

    cout << "Bounding Box: ";
    cout << minX << " <= x <= " << maxX << ", ";
    cout << minY << " <= y <= " << maxY << ", ";
    cout << minZ << " <= z <= " << maxZ << endl; 
}

void Model::printAll() {
    Point meanPoint = avgPoint();
    Point stdDevPoint = stdDev();

    cout << numPoints << " vertices, " << numFaces << " polygons" << endl;
    cout << "Mean Vertex = (" << meanPoint.getX() << ", " << meanPoint.getY() << ", " << meanPoint.getZ() << ")" << endl;
    printBoundingBox();
    cout << "Standard Deviations: x = " << stdDevPoint.getX() << ", y = " << stdDevPoint.getY() << ", z = " << stdDevPoint.getZ() << endl;
}