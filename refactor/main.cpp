#include <iostream>
#include <string.h>

using std::string;
using std::cout;
using std::endl;    

#include "Model.h"
#include "Camera.h"
#include "Scene.h"
#include "Linear.h"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Usage: raytracer <scenefile>" << endl;
        return 0;
    }

    Linear linear = Linear();
    Scene scene = Scene(argv[1], &linear);
    scene.genDistances();
    scene.write(argv[2]); 

	return 0;
}

