#include <iostream>
#include <string.h>

using std::string;
using std::cout;
using std::endl;    

#include "Model.h"
#include "Camera.h"

int main(int argc, char* argv[]) {
    if(argc < 4) {
        cout << "Usage: raytracer <CAMERA_MODEL> <PLY_MODEL(S)> <OUTFILE_NAME>" << endl;
        return 1;
    }
    string cameraFilename (argv[1]);
    string outfileName (argv[argc - 1]);

    int numModels = argc - 3;
    Model* models [numModels];
    for(int i = 2; i < argc-1; i++)
        models [i - 2] = new Model(argv[i]);

    Camera* camera = new Camera(argv[1]);
    camera->printCamera();
    
    // Clean memory
    for(int i = 0; i < numModels; i++)
        delete models[i];
    delete camera;

	return 0;
}

