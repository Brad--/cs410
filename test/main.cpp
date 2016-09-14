// main.cpp
#include <iostream>

#include "Pixel.t"

int main(int argc, char* argv[]){
	Pixel <float> p = Pixel<float>(1.0, 2.0, 3.0);
	std::cout << p.getRed() << "wtf" << std::endl;

	return 0;
}