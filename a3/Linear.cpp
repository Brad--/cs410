#include <Linear.h>
#include <iostream>
using std::cout;
using std::endl;
#include <math.h>
using std::sqrt;

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
    double mag = length(v, length);
    return scalar(v, mag, length);
}

double Linear::dot(double* v1, double* v2, int length){
    double sum = 0;
    for(int i = 0; i < length; i++) {
        sum += v1[i] * v2[i];
    }
    return sum;
}

double Linear::length(double* v, int length) {
    double sum = 0;
    for(int i = 0; i < length; i++) {
        sum += v[i] * v[i];
    }
    return sqrt(sum);
}