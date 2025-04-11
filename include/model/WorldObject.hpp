#ifndef WORLD_OBJECT_H
#define WORLD_OBJECT_H
#include <glm/glm.hpp>

#include "model/Model.hpp"

class WorldObject {
public:
  WorldObject();
  WorldObject(std::string const &path);
  void Draw();
  void Draw(Shader &shader);
  glm::vec3 getPosition() const;
  void setPosition(const glm::vec3 position);
  glm::vec3 getScale() const;
  void setScale(const glm::vec3 scale);

private:
  Model m_model;
  glm::vec3 m_position = glm::vec3(0.0f);
  glm::vec3 m_scale = glm::vec3(1.0f);
};

#endif
