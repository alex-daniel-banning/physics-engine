#include "model/WorldObject.hpp"
#include <glm/gtc/matrix_transform.hpp>

WorldObject::WorldObject(std::string const &path) { m_model = Model(path); }

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
