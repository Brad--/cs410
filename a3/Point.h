#ifndef POINT_H_INCLUDE
#define POINT_H_INCLUDE
class Point {
public:
    Point(double x, double y, double z);
    inline Point() {x = 0.0; y = 0.0; z = 0.0;}

    inline double getX() const {return x;}
    inline double getY() const {return y;}
    inline double getZ() const {return z;}

    void setX(double u_x);
    void setY(double u_y);
    void setZ(double u_z);
    void setXYZ(double u_x, double u_y, double y_z);

    void translate(double deltaX, double deltaY, double deltaZ);

    void print();

private:
    double x, y, z;
};

#endif