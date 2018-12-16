#ifndef CAMERA_HPP
#define CAMERA_HPP
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
    public:
        Camera(int width = 800, int height = 600);
        glm::mat4 getView();
        glm::mat4 getProjection();
        glm::vec3& getPos();
        void setCameraSpeed(float speed);
        void moveFoward(float dt);
        void moveBackward(float dt);
        void moveRight(float dt);
        void moveLeft(float dt);
        void moveUp(float dt);
        void moveDown(float dt);
        void update();
        void updateCameraDirection(double xpos, double ypos);
        void zoom(double yoffset);
        ~Camera();
    private:
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 cameraPos;
        glm::vec3 cameraFront;
        glm::vec3 cameraUp;
        float cameraSpeed, lastX, lastY;
        float mouseSensitivity;
        float pitch, yaw;
        bool firstMouse;
        float fieldOfView;
        int screenWidth, screenHeight;
};
#endif