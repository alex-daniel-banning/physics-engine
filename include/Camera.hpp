#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    public:
        Camera();
        void updateScreenDimensions(const unsigned int width, const unsigned int height);
        glm::mat4 getProjection();
        glm::mat4 getViewMatrix();
        void setLastMousePos(double xpos, double ypos);
        float getLastX();
        float getLastY();
        bool isFirstMouse();
        void setFirstMouse(bool value);
        float getYaw();
        void setYaw(float yaw);
        float getPitch();
        void setPitch(float pitch);
        float getFov();
        void setFov(float fov);
        unsigned int getScreenWidth();
        unsigned int getScreenHeight();
        float getSpeed();
        glm::vec3 getFront();
        void setFront(const glm::vec3 front);
        glm::vec3 getUp();
        glm::vec3 getPosition();
        void setPosition(const glm::vec3 position);

    private:
        const unsigned int SCR_WIDTH = 1600;
        const unsigned int SCR_HEIGHT = 1200;
        float m_lastX = SCR_WIDTH / 2.0f;
        float m_lastY = SCR_HEIGHT / 2.0f;
        float m_yaw = -90.0f;
        float m_pitch = 0.0f;
        bool m_firstMouse = true;
        double m_fov = 45.0f;
        glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 7.0f);
        glm::vec3 m_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        float m_nearPlane = 0.1f;
        float m_farPlane = 100.0f;
        glm::mat4 m_projection = glm::perspective(glm::radians((float)m_fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, m_nearPlane, m_farPlane);
        float m_sensitivity = 0.05f;
        float m_speed = 6.0f;
};

#endif
