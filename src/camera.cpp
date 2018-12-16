#include "camera.hpp"
#include <iostream>

Camera::Camera(int width, int height)
{
    cameraPos = glm::vec3(0.0f, 0.0f,  3.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
    cameraSpeed = 2.5f;
    fieldOfView = 45.0f;
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    projection = glm::perspective(glm::radians(fieldOfView), (float)width / (float)height, 0.1f, 100.0f);
    lastX = (float) width / 2.0;
    lastY = (float) height / 2.0;
    mouseSensitivity = 0.005f;
    pitch = 0.0f;
    yaw = -90.0f;
    firstMouse = true;
    screenWidth = width;
    screenHeight = height;
}
glm::mat4 Camera::getView()
{
    return view;
}
glm::mat4 Camera::getProjection()
{
    return projection;
}
glm::vec3& Camera::getPos()
{
    return cameraPos;
}
void Camera::setCameraSpeed(float speed)
{
    cameraSpeed = speed;
}
void Camera::moveFoward(float dt)
{
    std::cout << "W pressed\n";
    cameraPos += (cameraSpeed * dt) * cameraFront;
}
void Camera::moveBackward(float dt)
{
    std::cout << "S pressed\n";
    cameraPos -= (cameraSpeed * dt) * cameraFront;
}
void Camera::moveRight(float dt)
{
    std::cout << "D pressed\n";
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * (cameraSpeed * dt);
}
void Camera::moveLeft(float dt)
{
    std::cout << "A pressed\n";
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * (cameraSpeed * dt);
}
void Camera::moveUp(float dt)
{
    std::cout << "R pressed\n";
    cameraPos += (cameraSpeed * dt) * cameraUp;
}
void Camera::moveDown(float dt)
{
    std::cout << "F pressed\n";
    cameraPos -= (cameraSpeed * dt) * cameraUp;
}
void Camera::update()
{
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    projection = glm::perspective(glm::radians(fieldOfView), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
}
void Camera::updateCameraDirection(double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch =  89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
    
    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    cameraFront = glm::normalize(front);
}
void Camera::zoom(double yoffset)
{
    if(fieldOfView >= 1.0f && fieldOfView <= 45.0f)
        fieldOfView -= yoffset;
    if(fieldOfView <= 1.0f)
        fieldOfView = 1.0f;
    if(fieldOfView >= 45.0f)
        fieldOfView = 45.0f;
}
Camera::~Camera()
{
    //Do nothing for now
}