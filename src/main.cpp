#include <cmath>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "Camera.hpp"
#include "Shader.hpp"
#include "ProjectRoot.hpp"
#include "model/Model.hpp"
#include "model/WorldObject.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const *path);

// settings
unsigned int DEFAULT_SCREEN_WIDTH = 1600;
unsigned int DEFAULT_SCREEN_HEIGHT = 1200;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera = Camera(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
WorldObject worldObject;

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

  unsigned int woodTexture =
      loadTexture(ProjectRoot::getPath("/resources/container.jpg").c_str());
  Shader basicShader(
      ProjectRoot::getPath("/resources/shaders/basic_shader.vert"),
      ProjectRoot::getPath("/resources/shaders/basic_shader.frag"));
  Shader lightSourceShader(
      ProjectRoot::getPath("/resources/shaders/basic_shader.vert"),
      ProjectRoot::getPath("/resources/shaders/light_source_shader.frag"));
  Shader simpleDepthShader(
      ProjectRoot::getPath("/resources/shaders/simple_depth_shader.vert"),
      ProjectRoot::getPath("/resources/shaders/simple_depth_shader.frag"));

  glm::vec3 sphereColor = glm::vec3(0.8f, 0.0f, 0.0f);

  Model light = Model(ProjectRoot::getPath(
      "/resources/models/smooth_sphere/smooth_sphere.obj"));
  glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 lightPos = glm::vec3(2.0f, 2.0f, 1.0f);
  worldObject = WorldObject(ProjectRoot::getPath(
      "/resources/models/smooth_sphere/smooth_sphere.obj"));
  WorldObject plane =
      WorldObject(ProjectRoot::getPath("/resources/models/plane/plane.obj"));
  plane.setPosition(glm::vec3(0.0f, -2.0f, 0.0f));
  plane.setScale(glm::vec3(100.0f));
  glm::vec3 planeColor = glm::vec3(0.0f, 0.1f, 0.1f);
  camera.lookAt(worldObject.getPosition());

  /* Shadow stuff */
  unsigned int depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);

  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

  unsigned int depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
               SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);

    /*
    // 1. first render to depth map
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    ConfigureShaderAndMatrices();
    RenderScene();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // 2. then render scene as normal with shadow mapping (using depth map)
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ConfigureShaderAndMatrices();
    glBindTexture(GL_TEXTURE_2D, depthMap);
    RenderScene();
    */

    /*
    float near_plane = 1.0f, far_plane = 7.5f;
    glm::mat4 lightProjection =
        glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView =
        glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    simpleDepthShader.use();
    simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    worldObject.Draw(simpleDepthShader);
    plane.Draw(simpleDepthShader);
    /* End shadow stuff */

    // rendering commands here
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    basicShader.use();
    basicShader.setMat4("projection", camera.getProjection());
    basicShader.setMat4("view", camera.getViewMatrix());
    basicShader.setVec3("objectColor", sphereColor);
    basicShader.setVec3("lightColor", lightColor);
    basicShader.setVec3("lightPos", lightPos);
    basicShader.setBool("useTexture", false);
    worldObject.Draw(basicShader);

    basicShader.setVec3("objectColor", planeColor);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    basicShader.setInt("texture_diffuse1", 0);
    basicShader.setBool("useTexture", true);
    plane.Draw(basicShader);
    // glBindTexture(GL_TEXTURE_2D, 0);
    //  GLenum err;
    //  while ((err = glGetError()) != GL_NO_ERROR) {
    //    std::cout << "OpenGL error: " << err << std::endl;
    //  }

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

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    camera.lookAt(worldObject.getPosition());
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (camera.isFirstMouse()) {
    camera.setLastMousePos(xpos, ypos);
    camera.setFirstMouse(false);
  }
  float xoffset = xpos - camera.getLastX();
  float yoffset = camera.getLastY() - ypos;
  camera.setLastMousePos(xpos, ypos);

  const float sensitivity = 0.03f;
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

unsigned int loadTexture(char const *path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }
  return textureID;
}
