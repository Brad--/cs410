#include "Camera.h"

using std::ifstream;
using std::cout;
using std::endl;
#include <string>
#include <cstring>

Camera::Camera(string filename) {
    char cFilename [256];

    ifstream file(cFilename);
    read(file);
    file.close();
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
                eye[i] = token = strtok(NULL, " ");
            }
        }
        else if (strcmp(token, "look") == 0) {
            for(int i = 0; i < 3; i++) {
                look[i] = token = strtok(NULL, " ");
            }
        }
        else if (strcmp(token, "up") == 0) {
            for(int i = 0; i < 3; i++) {
                up[i] = token = strtok(NULL, " ");
            }
        }
        else if (strcmp(token, "d") == 0) {
            d = token = strtok(NULL, " ");
        }
        else if (strcmp(token, "bounds") == 0) {
            for(int i = 0; i < 4; i++) {
                bounds[i] = token = strtok(NULL, " ");
            }
        }
        else if (strcmp(token, "res") == 0) {
            for(int i = 0; i < 2; i++) {
                res[i] = token = strtok(NULL, " ");
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
    cout << "Res: " << res[0] << ", " << res[1] << endl;
}