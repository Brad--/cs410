#include "Camera.h"
#include "Linear.h"

using std::ifstream;
using std::cout;
using std::endl;
#include <string>
using std::string;
using std::stod;
#include <cstring>

Camera::Camera(string filename, Linear* inc_l) {
    l = inc_l;
    char cFilename [256];
    std::strcpy(cFilename, filename.c_str());

    ifstream file(cFilename);
    read(file);
    file.close();
}

Camera::~Camera() { }

// Should throw 0 or something that isn't -1 because -1 causes an error as defined by me
// This is leaking like a madman, write a class for vectors ya dingus
double Camera::throwRay(int x, int y) {
    // Returns -1 if no intersection, and distance of the intersection if the intersection exists
    // double left  = bounds[0];
    // double btm   = bounds[1];
    // double right = bounds[2];
    // double top   = bounds[3];

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
    
    delete [] temp;
    delete [] temp2;
    delete [] pixpt;
    delete [] zAxis;
    delete [] xAxis;
    delete [] yAxis;
    delete [] ray;

    // Distance should be the distance from the image plane to the object
    return -1; // 2 because otherwise it triggers the unset tmin/tmax
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