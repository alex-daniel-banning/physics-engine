#include <Camera.hpp>

Camera::Camera(const unsigned int scr_width, const unsigned int scr_height)
    : m_screenWidth(scr_width), m_screenHeight(scr_height),
      m_lastX(scr_width / 2.0f), m_lastY(scr_height / 2.0f), m_yaw(-90.0f),
      m_pitch(0.0f), m_firstMouse(true), m_fov(45.0f), m_speed(6.0f),
      m_sensitivity(0.05f), m_nearPlane(0.1f), m_farPlane(100.0f),
      m_position(glm::vec3(0.0f, 0.0f, 7.0f)),
      m_cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
      m_cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)) {
  m_projection = glm::perspective(glm::radians(m_fov),
                                  (float)m_screenWidth / (float)m_screenHeight,
                                  m_nearPlane, m_farPlane);
}

void Camera::updateScreenDimensions(const unsigned int width,
                                    const unsigned int height) {
  m_projection =
      glm::perspective(glm::radians(m_fov), (float)width / (float)height,
                       m_nearPlane, m_farPlane);
}

glm::mat4 Camera::getProjection() const { return m_projection; }

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(m_position, m_position + m_cameraFront, m_cameraUp);
}

void Camera::setPosition(const glm::vec3 position) { m_position = position; }

glm::vec3 Camera::getPosition() const { return m_position; }

float Camera::getSpeed() const { return m_speed; }

float Camera::getFov() const { return m_fov; }

void Camera::setFov(float fov) {
  m_fov = fov;
  m_projection = glm::perspective(glm::radians(m_fov),
                                  (float)m_screenWidth / (float)m_screenHeight,
                                  m_nearPlane, m_farPlane);
}

void Camera::setLastMousePos(double xpos, double ypos) {
  m_lastX = xpos;
  m_lastY = ypos;
}

float Camera::getLastX() const { return m_lastX; }

float Camera::getLastY() const { return m_lastY; }

bool Camera::isFirstMouse() const { return m_firstMouse; }

void Camera::setFirstMouse(bool value) { m_firstMouse = value; }

float Camera::getYaw() const { return m_yaw; }

void Camera::setYaw(float yaw) { m_yaw = yaw; }

float Camera::getPitch() const { return m_pitch; }

void Camera::setPitch(float pitch) { m_pitch = pitch; }

glm::vec3 Camera::getFront() const { return m_cameraFront; }

void Camera::setFront(const glm::vec3 front) { m_cameraFront = front; }

glm::vec3 Camera::getUp() const { return m_cameraUp; }
