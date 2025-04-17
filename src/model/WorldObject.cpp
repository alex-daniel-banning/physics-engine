#include "model/WorldObject.hpp"
#include <glm/gtc/matrix_transform.hpp>

WorldObject::WorldObject() {}
WorldObject::WorldObject(std::string const &path) { m_model = Model(path); }

void WorldObject::Draw() { this->m_model.Draw(); }

// todo, consider passing shader as const
// Need to set the projection/view matrices and uniforms in the shader before
// passing.
void WorldObject::Draw(Shader &shader) {
  shader.use();
  glm::mat4 modelTransform = glm::mat4(1.0f);
  modelTransform = glm::translate(modelTransform, m_position);
  //  todo, add rotation
  modelTransform = glm::scale(modelTransform, m_scale);
  shader.setMat4("model", modelTransform);
  this->m_model.Draw(shader);
}

glm::vec3 WorldObject::getPosition() const { return m_position; }

void WorldObject::setPosition(const glm::vec3 position) {
  m_position = position;
}

glm::vec3 WorldObject::getVelocity() const { return m_velocity; }

void WorldObject::setVelocity(const glm::vec3 velocity) {
  m_velocity = velocity;
}

glm::vec3 WorldObject::getScale() const { return m_scale; }

void WorldObject::setScale(const glm::vec3 scale) { m_scale = scale; }
