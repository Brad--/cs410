#include <Linear.h>
#include <iostream>
using std::cout;
using std::endl;
#include <math.h>

double* Linear::scalar(double* v, double scalar, int length) {
    double* scaled = new double[length];

    for(int i = 0; i < length; i++) {
        scaled[i] = v[i] * scalar;
    }  
    return scaled;
}

double* Linear::add(double* v1, double* v2, int length) {
    double* result = new double[length];

    for(int i = 0; i < length; i++) {
        result[i] = v1[i] + v2[i];
    }
    return result;
}

double* Linear::subtract(double* v1, double* v2, int length) {
    double* result = new double[length];

    for(int i = 0; i < length; i++) {
        result[i] = v1[i] - v2[i];
    }
    return result;
}

double* Linear::unit(double* v, int length) {
    double mag = vLength(v, length);
    return scalar(v, mag, length);
}

double Linear::dot(double* v1, double* v2, int length){
    double sum = 0;
    for(int i = 0; i < length; i++) {
        sum += v1[i] * v2[i];
    }
    return sum;
}

double Linear::vLength(double* v, int length) {
    double sum = 0;
    for(int i = 0; i < length; i++) {
        sum += v[i] * v[i];
    }
    return sqrt(sum);
}

// Specific function for cross3 because dat loop unrolling is OP
// Tested
double* Linear::cross3(double* v1, double* v2) {
    double* result = new double[3];

    result[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
    result[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
    result[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);

    return result;
}