#ifndef CAMERA_H_INCLUDE
#define CAMERA_H_INCLUDE

#include <iostream>

class Camera {
public:
    Camera(string filename);
    ~Camera();

    void read(std::ifstream& file);
    void printCamera();

    inline double* getEye()    {return &eye;}
    inline double* getLook()   {return &look;}
    inline double* getUp()     {return &up;}
    inline double* getBounds() {return &bounds;}
    inline double* getRes()    {return &res;}
    inline double  getD()      {return d;}

private:
    double eye [3];
    double look [3];
    double up [3];
    double bounds [4];
    double res [2];

    double d;
};

#endif