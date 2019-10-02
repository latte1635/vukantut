#pragma once
#include "Mesh.h"

class GameObject {
private:
    Mesh mesh;
    glm::vec3 position;
    glm::vec3 rotation;
public:
    GameObject(Mesh &mesh, glm::vec3 position, glm::vec3 rotation);

    ~GameObject();
};