#ifndef LINEAR_H_INCLUDE
#define LINEAR_H_INCLUDE

class Linear{
public:
    double* scalar(double* vector, double scalar, int length);
    double* add(double* v1, double* v2, int length);
    double* subtract(double* v1, double* v2, int length);
    double* unit(double* v1, int length);
    double  dot(double* v1, double* v2, int length);
    double  vLength(double* v1, int length);
    double* cross3(double* v1, double* v2);

    void makeUnit(double* v, int lenth);
private:
};

#endif