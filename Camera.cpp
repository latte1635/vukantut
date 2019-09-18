//
// Created by latte on 14/09/2019.
//

#include "Camera.h"

Camera* Camera::instance = nullptr;

Camera *Camera::getInstance() {
    if (instance == nullptr)
        instance = new Camera(glm::vec3(1.0f,1.0f,1.0f));
    return instance;
}

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up) :
        _position(position), _target(target), _up(up), _fov(60.0f) {
}

Camera::Camera(glm::vec3 position) :
    _position(position), _target(glm::vec3(0.0f, 0.0f, 0.0f)), _up(glm::vec3(0.0f, 0.0f, 1.0f)), _fov(60.0f) {
}

Camera::~Camera() {

}



glm::vec3 Camera::getPos() {
    return _position;
}

glm::vec3 Camera::getTarget() {
    return _target;
}

glm::vec3 Camera::getUp() {
    return _up;
}

float Camera::getFov() {
    return _fov;
}

void Camera::move(glm::vec3 change) {
    _position += change;
}

void Camera::moveTarget(glm::vec3 change) {
    _target += change;
}

void Camera::changeFov(float change) {
    _fov += change;
}
