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
    read(file);
}
bool Model::read(ifstream& file) {
    if (file.fail()) {
        file.clear();
        return false;
    }

    string curr;
    char *token;
    char *currChars;

    while(!file.eof()) {
        getline(file, curr);
        currChars = const_cast<char*>(curr.c_str());
        token = strtok(currChars, " ");

        if(token[0] != '#' && curr.find("/") != std::string::npos) {
            // Read vertex line
            if(strcmp(token, "v") == 0) {
                Point tempPoint = Point();

                token = strtok(NULL, " ");
                tempPoint.setX(stod(token));
                token = strtok(NULL, " ");
                tempPoint.setY(stod(token));
                token = strtok(NULL, " ");
                tempPoint.setZ(stod(token));

                points.push_back(tempPoint);
            }
            // Read texture coordinate
            if(strcmp(token, "vt") == 0) {
                // do nothing
            }
            // Read surface normal (Not necessarily unit)
            if(strcmp(token, "vn") == 0) {

            }
            // Read parameter space vertices (idk what that means)
            if(strcmp(token, "vp") == 0) {
                // do nothings
            }
            // Read face element (also don't know this)
            if(strcmp(token, "f") == 0) {
                Face tempFace = Face();
                vector<Point> facePoints;
                int pointPos = -1;

                token = strtok(NULL, " ");
                pointPos = stoi(token);
                facePoints.push_back(points[pointPos]);

                token = strtok(NULL, " ");
                pointPos = stoi(token);
                facePoints.push_back(points[pointPos]);

                token = strtok(NULL, " ");
                pointPos = stoi(token);
                facePoints.push_back(points[pointPos]);

                faces.push_back(tempFace);
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
        avgX += points[i].getX();
        avgY += points[i].getY();
        avgZ += points[i].getZ();
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
        devX += pow((points[i].getX() - mean.getX()), 2);
        devY += pow((points[i].getY() - mean.getY()), 2);
        devZ += pow((points[i].getZ() - mean.getZ()), 2);
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
        currPoint = &points[i];
        currPoint->setX(currPoint->getX() - meanPoint.getX());
        currPoint->setY(currPoint->getY() - meanPoint.getY());
        currPoint->setZ(currPoint->getZ() - meanPoint.getZ());
    }
}

void Model::whiten() {
    Point stdDevPoint = stdDev();
    Point *currPoint;
    for(unsigned int i = 0; i < points.size(); i++) {
        currPoint = &points[i];
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
    for(int i = 0; i < numPoints; i++) {
        points[i].translate(x, y, z);
    }
}

void Model::printPoints() {
    double tempX, tempY, tempZ;
    for(int i = 0; i < numPoints; i++) {
        tempX = points[i].getX();
        tempY = points[i].getY();
        tempZ = points[i].getZ();
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
    for(int i = 0; i < numPoints; i++) {
        tempX = points[i].getX();
        tempY = points[i].getY();
        tempZ = points[i].getZ();

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