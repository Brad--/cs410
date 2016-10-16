#include "Camera.h"

using std::ifstream;
using std::cout;
using std::endl;
#include <string>
using std::string;
using std::stod;
#include <cstring>

Camera::Camera(string filename) {
    char cFilename [256];
    std::strcpy(cFilename, filename.c_str());

    ifstream file(cFilename);
    read(file);
    file.close();
}

Camera::~Camera() { }

double Camera::throwRay(int x, int y) {
    // Throws a ray at pixel (x,y) and determines if the ray intersects a model
    // Returns -1 if no intersection, and distance of the intersection if the intersection exists
    // if throwRay = -1, the user should use the default background color, (239, 239, 239)

    return -1;
}

void Camera::read(ifstream& file) {
    if (file.fail()) {
        file.clear();
        return;
    }
    string curr;
    char* token;
    char* currChars;

    while (!file.eof()){
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
            for(int i = 0; i < 4; i++) {
                token = strtok(NULL, " ");
                bounds[i] = stod(token);
            }
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
    }
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
    cout << "Bounds: ";
    for(int i = 0; i < 4; i++) {
        cout << bounds[i]<< ", ";
    }
    cout << endl << "Res: " << res[0] << ", " << res[1] << endl;
}