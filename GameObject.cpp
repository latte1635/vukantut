#include "../GameObject.h"

GameObject::GameObject(Mesh &mesh, glm::vec3 position, glm::vec3 rotation):
mesh(nullptr),
position(glm::vec3(0.0f, 0.0f, 0.0f)),
rotation(glm::vec3(0.0f, 0.0f, 0.0f))
{}

GameObject::~GameObject() = default;
