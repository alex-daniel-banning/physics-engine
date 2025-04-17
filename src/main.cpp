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
#include "model/WorldObject.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const *path);
void renderQuad();
void renderFloor(const Shader &shader);
void renderCube();

// settings
unsigned int DEFAULT_SCREEN_WIDTH = 1600;
unsigned int DEFAULT_SCREEN_HEIGHT = 1200;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera = Camera(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
float cameraMinX = -40.0f;
float cameraMaxX = 40.0f;
float cameraMinY = 0.0f;
float cameraMaxY = 40.0f;
float cameraMinZ = -40.0f;
float cameraMaxZ = 40.0f;

// meshes
float borderMinX = -15.0f;
float borderMaxX = 15.0f;
float borderMinY = 0.0f;
float borderMaxY = 8.0f;
float borderMinZ = -15.0f;
float borderMaxZ = 15.0f;
std::vector<glm::vec3> corners = {
    {borderMinX, borderMinY, borderMinZ}, {borderMinX, borderMinY, borderMaxZ},
    {borderMinX, borderMaxY, borderMinZ}, {borderMinX, borderMaxY, borderMaxZ},
    {borderMaxX, borderMinY, borderMinZ}, {borderMaxX, borderMinY, borderMaxZ},
    {borderMaxX, borderMaxY, borderMinZ}, {borderMaxX, borderMaxY, borderMaxZ}};

unsigned int planeVAO;
float planeVertices[] = {
    // positions            // normals         // texcoords
    borderMinX, cameraMinY, borderMaxZ, 0.0f, 1.0f, 0.0f, 0.0f,  0.0f,
    borderMaxX, cameraMinY, borderMaxZ, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
    borderMinX, cameraMinY, borderMinZ, 0.0f, 1.0f, 0.0f, 0.0f,  25.0f,

    borderMinX, cameraMinY, borderMinZ, 0.0f, 1.0f, 0.0f, 0.0f,  25.0f,
    borderMaxX, cameraMinY, borderMaxZ, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
    borderMaxX, cameraMinY, borderMinZ, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f};

/* todo list
 * Add hitboxes to world objects
 *  - spheres
 *  - rectangles
 * Render objects with solid colors
 *  - Shader should use bool uniform 'useTexture'
 */
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

  camera.setPosition(glm::vec3(borderMaxX, borderMaxY / 2, borderMaxZ));
  camera.lookAt(glm::vec3(0.0f, camera.getPosition().y, 0.0f));

  Shader simpleDepthShader(
      ProjectRoot::getPath("/resources/shaders/simple_depth_shader.vert"),
      ProjectRoot::getPath("/resources/shaders/simple_depth_shader.frag"));
  Shader shader(ProjectRoot::getPath("/resources/shaders/shadows.vert"),
                ProjectRoot::getPath("/resources/shaders/shadows.frag"));
  Shader basicShader(
      ProjectRoot::getPath("/resources/shaders/basic_shader.vert"),
      ProjectRoot::getPath("/resources/shaders/basic_shader.frag"));

  // todo, replace with world object
  // plane VAO
  unsigned int planeVBO;
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);
  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glBindVertexArray(0);

  unsigned int woodTexture =
      loadTexture(ProjectRoot::getPath("/resources/wood.png").c_str());

  const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
  unsigned int depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);
  // create depth texture
  unsigned int depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
               SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  // attach depth texture as FBO's depth buffer
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  shader.use();
  shader.setBool("useTexture", true);
  shader.setInt("diffuseTexture", 0);
  shader.setInt("shadowMap", 1);

  WorldObject sphere(ProjectRoot::getPath(
      "/resources/models/smooth_sphere/smooth_sphere.obj"));
  sphere.setScale(glm::vec3(0.5f));
  sphere.setPosition(glm::vec3(0.0f, 2.5f, 0.0f));
  sphere.setVelocity(glm::vec3(40.0f, 40.0f, 20.0f));

  glm::vec3 lightPos = glm::vec3(borderMaxX, borderMaxY, borderMaxZ);
  WorldObject lightOrb(
      ProjectRoot::getPath("/resources/models/sphere/sphere.obj"));

  glm::mat4 lightProjection, lightView;
  glm::mat4 lightSpaceMatrix;
  float near_plane = 0.1f,
        far_plane = std::sqrt(std::pow(borderMaxX - borderMinX, 2) +
                              std::pow(borderMaxY - borderMinY, 2) +
                              std::pow(borderMaxZ - borderMinZ, 2));
  lightView =
      glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  // transform world corners into light space
  std::vector<glm::vec3> lightSpaceCorners;
  for (glm::vec3 &corner : corners) {
    glm::vec4 transformed = lightView * glm::vec4(corner, 1.0f);
    lightSpaceCorners.push_back(glm::vec3(transformed));
  }
  // find min/max of lightspace
  float x1 = std::numeric_limits<float>::max();
  float x2 = std::numeric_limits<float>::lowest();
  float y1 = std::numeric_limits<float>::max();
  float y2 = std::numeric_limits<float>::lowest();
  for (glm::vec3 &v : lightSpaceCorners) {
    x1 = std::min(x1, v.x);
    x2 = std::max(x2, v.x);
    y1 = std::min(y1, v.y);
    y2 = std::max(y2, v.y);
  }

  lightProjection = glm::ortho(x1, x2, y1, y2, near_plane, far_plane);
  glm::vec3 lightDirection = glm::normalize(glm::vec3(0.0f) - lightPos);
  lightSpaceMatrix = lightProjection * lightView;

  bool firstErr = false;
  while (!glfwWindowShouldClose(window)) {
    /*** per-frame time logic ***/
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    /*** Input ***/
    processInput(window);

    /*** World tick ***/
    glm::vec3 newPosition =
        sphere.getPosition() + (deltaTime * sphere.getVelocity());
    if (newPosition.x <= borderMinX) {
      sphere.setVelocity(sphere.getVelocity() * glm::vec3(-1.0f, 1.0f, 1.0f));
      newPosition.x = borderMinX;
    }
    if (newPosition.x >= borderMaxX) {
      sphere.setVelocity(sphere.getVelocity() * glm::vec3(-1.0f, 1.0f, 1.0f));
      newPosition.x = borderMaxX;
    }
    if (newPosition.y <= borderMinY) {
      sphere.setVelocity(sphere.getVelocity() * glm::vec3(1.0f, -1.0f, 1.0f));
      newPosition.y = borderMinY;
    }
    if (newPosition.y >= borderMaxY) {
      sphere.setVelocity(sphere.getVelocity() * glm::vec3(1.0f, -1.0f, 1.0f));
      newPosition.y = borderMaxY;
    }
    if (newPosition.z <= borderMinZ) {
      sphere.setVelocity(sphere.getVelocity() * glm::vec3(1.0f, 1.0f, -1.0f));
      newPosition.z = borderMinZ;
    }
    if (newPosition.z >= borderMaxZ) {
      sphere.setVelocity(sphere.getVelocity() * glm::vec3(1.0f, 1.0f, -1.0f));
      newPosition.z = borderMaxZ;
    }
    sphere.setPosition(newPosition);

    /*** Rendering commands here ***/
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Render shadows */
    simpleDepthShader.use();
    simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    renderFloor(simpleDepthShader);
    sphere.Draw(simpleDepthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // reset viewport
    glViewport(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Render scene */
    shader.use();
    shader.setBool("useTexture", true);
    glm::mat4 projection = glm::perspective(glm::radians(camera.getFov()),
                                            (float)DEFAULT_SCREEN_WIDTH /
                                                (float)DEFAULT_SCREEN_HEIGHT,
                                            0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setVec3("viewPos", camera.getPosition());
    shader.setVec3("lightPos", lightPos);
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    shader.setVec3("lightDirection", lightDirection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    renderFloor(shader);
    shader.setBool("useTexture", false);
    shader.setVec3("color", glm::vec3(0.5f, 0.0f, 0.0f));
    sphere.Draw(shader);

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
    moveDirection += camera.getFront() * glm::vec3(1, 0, 1);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    moveDirection -= camera.getFront() * glm::vec3(1, 0, 1);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    moveDirection -=
        glm::normalize(glm::cross(camera.getFront(), camera.getUp()));
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    moveDirection +=
        glm::normalize(glm::cross(camera.getFront(), camera.getUp()));
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    moveDirection += glm::vec3(0.0f, 1.0f, 0.0f);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    moveDirection -= glm::vec3(0.0f, 1.0f, 0.0f);
  }

  if (glm::length(moveDirection) != 0.0f) {
    moveDirection = glm::normalize(moveDirection);
  }
  glm::vec3 newPosition = camera.getPosition() + (moveDirection * cameraSpeed);
  if (newPosition.x < cameraMinX)
    newPosition.x = cameraMinX;
  if (newPosition.x > cameraMaxX)
    newPosition.x = cameraMaxX;
  if (newPosition.y < cameraMinY + 0.3f)
    newPosition.y = cameraMinY + 0.3f;
  if (newPosition.y > cameraMaxY)
    newPosition.y = cameraMaxY;
  if (newPosition.z < cameraMinZ)
    newPosition.z = cameraMinZ;
  if (newPosition.z > cameraMaxZ)
    newPosition.z = cameraMaxZ;
  camera.setPosition(newPosition);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (camera.isFirstMouse()) {
    camera.setLastMousePos(xpos, ypos);
    camera.setFirstMouse(false);
  }
  float xoffset = xpos - camera.getLastX();
  float yoffset = camera.getLastY() - ypos;
  camera.setLastMousePos(xpos, ypos);

  const float sensitivity = 0.04f;
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

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() {
  if (quadVAO == 0) {
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void renderFloor(const Shader &shader) {
  // floor
  glm::mat4 model = glm::mat4(1.0f);
  shader.setMat4("model", model);
  glBindVertexArray(planeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube() {
  // initialize (if necessary)
  if (cubeVAO == 0) {
    float vertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
        // front face
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
        -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
                                                            // right face
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left
        // bottom face
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
    };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    // fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // link vertex attributes
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  // render Cube
  glBindVertexArray(cubeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}
