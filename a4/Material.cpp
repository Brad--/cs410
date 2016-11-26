#include "Material.h"

Material::Material() { }

Material::Material(string n) {
    name = n;
}

void Material::setDiffuse(double r, double g, double b) {
    diffuse[0] = r;
    diffuse[1] = g;
    diffuse[2] = b;
}

void Material::setSpecular(double r, double g, double b) {
    specular[0] = r;
    specular[1] = g;
    specular[2] = b;
}

bool Material::includes(int position) {
    return (position >= begin && position <= end);
}