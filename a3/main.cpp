#include <iostream>
#include <string.h>

using std::string;
using std::cout;
using std::endl;    

#include "Model.h"
#include "Camera.h"
#include "Scene.h"

int main(int argc, char* argv[]) {
    if(argc < 4) {
        cout << "Usage: raytracer <CAMERA_MODEL> <PLY_MODEL(S)> <OUTFILE_NAME>" << endl;
    }
    string cameraFilename (argv[1]);
    string outfileName (argv[argc - 1]);

    int numModels = argc - 3;
    Model* models [numModels];
    for(int i = 2; i < argc-1; i++)
        models [i - 2] = new Model(argv[i]);

    Camera* camera = new Camera(argv[1]);
    Scene* scene = new Scene();
    scene->distToDepth(1);

    // Clean memory
    for(int i = 0; i < numModels; i++)
        delete models[i];
    delete camera;
    delete scene;

	return 0;
}

