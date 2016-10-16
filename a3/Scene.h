#ifndef SCENE_H_INCLUDE
#define SCENE_H_INCLUDE

class Scene{
public:
    Scene();
    ~Scene();

    double* distToDepth(double d);
private:
    // Maximum and Minimum distance in the scene
    double tmin;
    double tmax;
};

#endif