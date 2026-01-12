#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    glm::vec3 cameraPos, cameraFront, Yup;
    float Fov, Yaw, Pitch, lastX, lastY;
    float SenseKey, SenseMouse, SenseScroll;
    bool firstMouse, restrictView;

    Camera(float fov = 45.0f, float yaw = -90.0f, float pitch = 0.0f, bool restrict = true,
        float sensekey = 4.0f, float sensemouse = 0.1f, float sensescroll = 4.0f);
    void Pause();
    void KeyboardMove(float deltaTime, char direction);
    void KeyboardRotate(float xoffset, float yoffset);
    void MouseRotate(float xpos, float ypos);
    void ScrollScale(float xoffset, float yoffset);
    glm::mat4 ComputeView();
    glm::mat4 ComputeProjection();
    glm::mat4 ComputeProjection0();
};

Camera::Camera(float fov, float yaw, float pitch, bool restrict, float sensekey, float sensemouse, float sensescroll) : Fov(fov), Yaw(yaw), Pitch(pitch),
        restrictView(restrict),SenseKey(sensekey), SenseMouse(sensemouse), SenseScroll(sensescroll), lastX(0.0f), lastY(0.0f), firstMouse(true) {
    cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    Yup = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Camera::KeyboardMove(float deltaTime, char direction) {
    float cameraSpeed = 4.0f * deltaTime; // adjust accordingly
    glm::vec3 cameraSide = glm::normalize(glm::cross(cameraFront, Yup));
    if (direction == 'W')
        cameraPos += cameraSpeed * cameraFront;
    if (direction == 'A')
        cameraPos -= cameraSpeed * cameraFront;
    if (direction == 'S')
        cameraPos -= cameraSpeed * cameraSide;
    if (direction == 'D')
        cameraPos += cameraSpeed * cameraSide;
}

void Camera::KeyboardRotate(float xoffset, float yoffset) {
    Yaw += xoffset * SenseMouse;
    Pitch += yoffset * SenseMouse;
    if (restrictView) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    cameraFront = glm::normalize(front);
}

void Camera::MouseRotate(float xpos, float ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    Yaw += (xpos - lastX) * SenseMouse;
    Pitch += (lastY - ypos) * SenseMouse;
    lastX = xpos;
    lastY = ypos;
    if (restrictView) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    cameraFront = glm::normalize(front);
}

void Camera::ScrollScale(float xoffset, float yoffset) {
    if (Fov >= 5.0f && Fov <= 85.0f)
        Fov -= SenseScroll * yoffset;
    if (Fov <= 5.0f)
        Fov = 5.0f;
    if (Fov >= 85.0f)
        Fov = 85.0f;
}

glm::mat4 Camera::ComputeView() {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, Yup);
}

glm::mat4 Camera::ComputeProjection() {
    return glm::perspective(glm::radians(Fov), 800.0f / 600.0f, 0.1f, 100.0f);
}

glm::mat4 Camera::ComputeProjection0() {
    return glm::perspective(glm::radians(Fov), 512.0f / 512.0f, 0.1f, 100.0f);
}

#endif