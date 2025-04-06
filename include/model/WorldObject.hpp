#ifndef WORLD_OBJECT_H
#define WORLD_OBJECT_H
#include <glm/glm.hpp>

#include "model/Model.hpp"

class WorldObject {
public:
  WorldObject(std::string const &path);
  void Draw(Shader &shader);

private:
  Model m_model;
  glm::vec3 m_position = glm::vec3(0.0f);
  glm::vec3 m_scale = glm::vec3(1.0f);
};

#endif
