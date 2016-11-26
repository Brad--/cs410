#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
using std::string;

class Material {
public:
    Material();
    Material(string n);

    inline double* getDiffuse() {return diffuse;}
    inline double* getSpecular() {return specular;}
    inline string getName() {return name;}

    inline void setBegin(int b) {begin = b;}
    inline void setEnd(int e) {end = e;}

    void setDiffuse(double r, double g, double b);
    void setSpecular(double r, double g, double b);

    bool includes(int position);
private:
    int begin, end;
    double diffuse[3], specular[3];
    string name;
};
#endif