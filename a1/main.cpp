#include <iostream>
#include <string.h>

using std::string;
using std::cout;
using std::endl;

#include "Model.h"

int main(int argc, char* argv[]) {
	if(argc < 2) {
		cout << "Usage: a1 <FILE_NAME>" << endl;
		return 1;
	}

	string filename (argv[1]);
    Model model = Model(filename);
    cout << "=== Before centering" << endl;
    model.printAll();

    model.center();
    cout << "=== After Centering" << endl;
    model.printAll();
    model.write(filename, "_centered");

    model.whiten();
    cout << "=== After Whitening" << endl;
    model.printAll();
    model.write(filename, "_rounded");

	return 0;
}