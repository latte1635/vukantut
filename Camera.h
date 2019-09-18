//
// Created by latte on 14/09/2019.
//

#ifndef VULKANTUTORIAL_CAMERA_H
#define VULKANTUTORIAL_CAMERA_H


#include <glm/glm.hpp>
#include <vector>

class Camera {
private:
    static Camera* instance;
    glm::vec3 _position;
    glm::vec3 _target;
    glm::vec3 _up;
    float _fov;
public:
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);
    explicit Camera(glm::vec3 position);
    ~Camera();

    static Camera* getInstance();
    glm::vec3 getPos();
    glm::vec3 getTarget();
    glm::vec3 getUp();
    float getFov();

    void move(glm::vec3 change);
    void moveTarget(glm::vec3 change);
    void changeFov(float change);
};

#endif //VULKANTUTORIAL_CAMERA_H
