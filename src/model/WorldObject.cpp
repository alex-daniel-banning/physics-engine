#include "model/WorldObject.hpp"
#include <glm/gtc/matrix_transform.hpp>

WorldObject::WorldObject(std::string const &path) {
    model = Model(path);
}

// todo, consider passing shader as const
// Need to set the projection/view matrices and uniforms in the shader before passing.
void WorldObject::Draw(Shader& shader) {
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    //std::cout << "position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
    // todo, add rotation
    model = glm::scale(model, m_scale);
    shader.setMat4("model", model);
    this->model.Draw(shader); // todo, make m_model
}
