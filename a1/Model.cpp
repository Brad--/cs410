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
    char cFilename [256];
    std::strcpy(cFilename, filename.c_str());

    ifstream file(cFilename);
    read(file);
    file.close();
}

Model::~Model() {
    if(points != NULL)
        free(points);
}

// Reads .ply file and stores appropriate information
// !!! Does not currently store property list !!!
bool Model::read(ifstream& file) {
    if (file.fail()) {
        file.clear();
        return false;
    }

    string headerTemp;
    string curr;
    char *token;
    char *currChars;

    // Note for future me: strtok returns a single token from a string based on 
    //  delimiter, so 'token' is a char*, not a char* array

    // Ghetto string.contains()
    while(curr.find("end_header") == std::string::npos) {
        getline(file, curr);
        headerTemp += curr;

        // Convert string into non const char*
        currChars = const_cast<char*>(curr.c_str());
        token = strtok(currChars, " ");

        // Don't append extra newline after end_header
        if(curr.find("end_header") == std::string::npos) 
            headerTemp += "\n";

        if(strcmp(token, "element") == 0) {
            // The strtok NULL thing reads the next token in the string for some reason
            token = strtok(NULL, " ");

            if(strcmp(token, "vertex") == 0) {
                token = strtok(NULL, " ");
                numPoints = stoi(token);
            } else if(strcmp(token, "face") == 0) {
                token = strtok(NULL, " ");
                numFaces = stoi(token);
            }
        }
    }
    header = headerTemp;

    points = (Point*)malloc(sizeof(Point) * numPoints);
    for(int i = 0; i < numPoints; i++) {
        double tempX, tempY, tempZ;

        getline(file, curr);
        currChars = const_cast<char*>(curr.c_str());

        token = strtok(currChars, " ");
        tempX = stod(token);
        token = strtok(NULL, " ");
        tempY = stod(token);
        token = strtok(NULL, " ");
        tempZ = stod(token);

        points[i] = Point(tempX, tempY, tempZ);
        // cout << "X: " << tempX << " Y: " << tempY << " Z: " << tempZ << endl;
    } // Done reading Points

    string tempFaces = "";
    while(!file.eof()) {
        getline(file, curr);
        tempFaces += curr + "\n"; // Don't add the endline in the last case
    } // Done reading Faces
    faces = tempFaces;

    // Debuggin'
    // cout << "Begin Faces:\n" << faces << endl;
    // cout << "Point0: " << points[0].getX() << endl;
    // cout << "Header w/o elements: " << headerTemp << endl;
    // cout << "Points: " << numPoints << endl;
    // cout << "Faces: " << numFaces << endl;

    return true;
}

bool Model::write(string filename, string ext) const {
    string fileTop = filename.substr(0, filename.size() - 4);
    const char *cFilename = (fileTop + ext + ".ply").c_str();
    std::ofstream outfile;
    outfile.open(cFilename);

    outfile << header << endl;

    for(int i = 0; i < numPoints; i++) {
        outfile << points[i].getX() << " " << points[i].getY() << " " << points[i].getZ() << endl;
    }

    outfile << faces;
    outfile.close();

    return true;
}

Point Model::avgPoint() const {
    double avgX = 0.0;
    double avgY = 0.0;
    double avgZ = 0.0;

    for(int i = 0; i < numPoints; i++) {
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
    for(int i = 0; i < numPoints; i++) {
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
    for(int i = 0; i < numPoints; i++) {
        currPoint = &points[i];
        currPoint->setX(currPoint->getX() - meanPoint.getX());
        currPoint->setY(currPoint->getY() - meanPoint.getY());
        currPoint->setZ(currPoint->getZ() - meanPoint.getZ());
    }
}

void Model::whiten() {
    Point stdDevPoint = stdDev();
    Point *currPoint;
    for(int i = 0; i < numPoints; i++) {
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