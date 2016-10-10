#include <iostream>
#include <string.h>

using std::string;
using std::cout;
using std::endl;

#include "Model.h"

int main(int argc, char* argv[]) {
    if(argc < 4) {
        cout << "Usage: raytracer <CAMERA_MODEL> <PLY_MODEL(S)> <OUTFILE_NAME>" << endl;
        return 1;
    }
    string camera_filename (argv[1]);
    string outfile_name (argv[argc - 1]);

    string model_files [argc - 3];
    for(int i = 2; i < argc-1; i++) {
        model_files[i - 2] = argv[i];
    }    

	return 0;
}