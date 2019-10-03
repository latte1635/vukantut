#pragma once
#include "Mesh.h"

class GameObject {
private:
    Mesh mesh;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

public:
    GameObject(Mesh &_mesh, glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale) :
    mesh(_mesh),
    position(_position),
    rotation(_rotation),
    scale(_scale){}

    ~GameObject() = default;

    void createMesh() {
        mesh.create();
    }

    std::vector<uint32_t> getIndices() {
        return mesh.getIndices();
    }

    std::vector<Vertex> getVertices() {
        return mesh.getVertices();
    }

    void move(glm::vec3 change) {
        position += change;
    }
    void rotate(glm::vec3 change) {
        rotation += change;
    }
    void resize(glm::vec3 change) {
        scale += change;
    }

    glm::vec3 getPosition() {
        return position;
    }

    glm::vec3 getRotation() {
        return rotation;
    }

    glm::vec3 getScale() {
        return scale;
    }
};