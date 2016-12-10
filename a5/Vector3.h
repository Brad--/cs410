#ifndef VECTOR3_H_INCLUDE
#define VECTOR3_H_INCLUDE

class Vector3 {
public:
    Vector3();
    Vector3(double scale);
    ~Vector3();

    void homogenize();

    Vector3 operator+(const Vector3& toAdd);
    double operator[](int index);
private:
    double points[3];
    // The homogenous part
    double scale;
};
#endif