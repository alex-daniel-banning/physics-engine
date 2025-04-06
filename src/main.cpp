// #include <Camera.hpp>
// #include <GLFW/glfw3.h>
// #include <ProjectRoot.hpp>
// #include <Shader.hpp>
// #include <cmath>
// #include <glad/glad.h>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #include <iostream>
// #include <model/Model.hpp>
// #include <model/Vertex.hpp>
// #include <model/WorldObject.hpp>
// #include <stb_image.h>

#include <iostream>
#include <cmath>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "model/Model.hpp"
#include "Shader.hpp"
#include "ProjectRoot.hpp"
#include "model/WorldObject.hpp"
#include "Camera.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// settings
unsigned int DEFAULT_SCREEN_WIDTH = 1600;
unsigned int DEFAULT_SCREEN_HEIGHT = 1200;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera = Camera(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  GLFWwindow *window = glfwCreateWindow(
      DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // tell stb_image to flip loaded texture's on the y-axis (before loading
  // model)
  stbi_set_flip_vertically_on_load(true);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE); // Enables face culling
  glCullFace(GL_BACK);    // Culls (hides) back faces
  glFrontFace(
      GL_CCW); // Counterclockwise (CCW) faces are considered front-facing

  Shader basicShader(
      ProjectRoot::getPath("/resources/shaders/basic_shader.vert"),
      ProjectRoot::getPath("/resources/shaders/basic_shader.frag"));
  Shader lightSourceShader(
      ProjectRoot::getPath("/resources/shaders/basic_shader.vert"),
      ProjectRoot::getPath("/resources/shaders/light_source_shader.frag"));

  glm::vec3 sphereColor = glm::vec3(0.8f, 0.0f, 0.0f);

  Model light = Model(ProjectRoot::getPath(
      "/resources/models/smooth_sphere/smooth_sphere.obj"));
  glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 lightPos = glm::vec3(2.0f, 2.0f, 1.0f);

  WorldObject worldObject(ProjectRoot::getPath(
      "/resources/models/smooth_sphere/smooth_sphere.obj"));

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);

    // rendering commands here
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    basicShader.use();
    basicShader.setMat4("projection", camera.getProjection());
    basicShader.setMat4("view", camera.getViewMatrix());
    basicShader.setVec3("objectColor", sphereColor);
    basicShader.setVec3("lightColor", lightColor);
    basicShader.setVec3("lightPos", lightPos);
    worldObject.Draw(basicShader);

    lightSourceShader.use();
    lightSourceShader.setMat4("projection", camera.getProjection());
    lightSourceShader.setMat4("view", camera.getViewMatrix());
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);
    lightModel = glm::scale(lightModel, glm::vec3(0.2f));
    lightSourceShader.setMat4("model", lightModel);
    light.Draw(lightSourceShader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  camera.updateScreenDimensions(width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  const float cameraSpeed = camera.getSpeed() * deltaTime;
  glm::vec3 moveDirection(0.0f);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    moveDirection += camera.getFront();
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    moveDirection -= camera.getFront();
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    moveDirection -=
        glm::normalize(glm::cross(camera.getFront(), camera.getUp()));
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    moveDirection +=
        glm::normalize(glm::cross(camera.getFront(), camera.getUp()));
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    moveDirection += glm::vec3(0.0f, 1.0f, 0.0f);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    moveDirection -= glm::vec3(0.0f, 1.0f, 0.0f);

  if (glm::length(moveDirection) != 0.0f) {
    moveDirection = glm::normalize(moveDirection);
  }
  camera.setPosition(camera.getPosition() + (moveDirection * cameraSpeed));
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (camera.isFirstMouse()) {
    camera.setLastMousePos(xpos, ypos);
    camera.setFirstMouse(false);
  }
  float xoffset = xpos - camera.getLastX();
  float yoffset = camera.getLastY() - ypos;
  camera.setLastMousePos(xpos, ypos);

  const float sensitivity = 0.05f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;
  camera.setYaw(camera.getYaw() + xoffset);
  camera.setPitch(camera.getPitch() + yoffset);

  if (camera.getPitch() > 89.0f)
    camera.setPitch(89.0f);
  if (camera.getPitch() < -89.0f)
    camera.setPitch(-89.0f);

  glm::vec3 direction;
  direction.x =
      cos(glm::radians(camera.getYaw())) * cos(glm::radians(camera.getPitch()));
  direction.y = sin(glm::radians(camera.getPitch()));
  direction.z =
      sin(glm::radians(camera.getYaw())) * cos(glm::radians(camera.getPitch()));
  camera.setFront(glm::normalize(direction));
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.setFov(camera.getFov() - (float)yoffset);
  if (camera.getFov() < 1.0f)
    camera.setFov(1.0f);
  if (camera.getFov() > 45.0f)
    camera.setFov(45.0f);
}
