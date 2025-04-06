#include <Camera.hpp>

Camera::Camera() {}

void Camera::updateScreenDimensions(const unsigned int width, const unsigned int height) {
    m_projection = glm::perspective(glm::radians((float)m_fov), (float)width / (float)height, m_nearPlane, m_farPlane);
}

glm::mat4 Camera::getProjection() {
    return m_projection;
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(m_position, m_position + m_cameraFront, m_cameraUp);
}

void Camera::setLastMousePos(double xpos, double ypos) {
    m_lastX = xpos;
    m_lastY = ypos;
}

bool Camera::isFirstMouse() {
    return m_firstMouse;
}

void Camera::setFirstMouse(bool value) {
    m_firstMouse = value;
}

float Camera::getYaw() {
    return m_yaw;
}

void Camera::setYaw(float yaw) {
    m_yaw = yaw;
}

float Camera::getPitch() {
    return m_pitch;
}

void Camera::setPitch(float pitch) {
    m_pitch = pitch;
}

float Camera::getFov() {
    return m_fov;
}

void Camera::setFov(float fov) {
    m_fov = fov;
    m_projection = glm::perspective(glm::radians((float)m_fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, m_nearPlane, m_farPlane);
}

unsigned int Camera::getScreenWidth() {
    return SCR_WIDTH;
}

unsigned int Camera::getScreenHeight() {
    return SCR_HEIGHT;
}

float Camera::getLastX() {
    return m_lastX;
}

float Camera::getLastY() {
    return m_lastY;
}

float Camera::getSpeed() {
    return m_speed;
}

glm::vec3 Camera::getFront() {
    return m_cameraFront;
}

void Camera::setFront(const glm::vec3 front) {
    m_cameraFront = front;
}

glm::vec3 Camera::getUp() {
    return m_cameraUp;
}

glm::vec3 Camera::getPosition() {
    return m_position;
}

void Camera::setPosition(const glm::vec3 position) {
    m_position = position;
}
