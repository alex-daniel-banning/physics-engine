#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  Camera(const unsigned int scr_width, const unsigned int scr_height);

  void updateScreenDimensions(const unsigned int width,
                              const unsigned int height);

  glm::mat4 getProjection() const;
  glm::mat4 getViewMatrix() const;

  void setPosition(const glm::vec3 position);
  glm::vec3 getPosition() const;
  float getSpeed() const;
  float getFov() const;
  void setFov(float fov);

  void setLastMousePos(double xpos, double ypos);
  float getLastX() const;
  float getLastY() const;
  bool isFirstMouse() const;
  void setFirstMouse(bool value);
  float getYaw() const;
  void setYaw(float yaw);
  float getPitch() const;
  void setPitch(float pitch);
  void lookAt(const glm::vec3 point);

  glm::vec3 getFront() const;
  void setFront(const glm::vec3 front);
  glm::vec3 getUp() const;

private:
  unsigned int m_screenWidth;
  unsigned int m_screenHeight;
  float m_lastX;
  float m_lastY;
  bool m_firstMouse;

  float m_yaw;
  float m_pitch;
  float m_fov;

  float m_speed;
  float m_sensitivity;
  float m_nearPlane;
  float m_farPlane;

  glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 7.0f);
  glm::vec3 m_front = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);

  glm::mat4 m_projection;
};

#endif
