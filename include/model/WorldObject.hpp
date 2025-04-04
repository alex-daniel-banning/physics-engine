#ifndef WORLD_OBJECT_H
#define WORLD_OBJECT_H
#include <glm/glm.hpp>
#include <model/Model.hpp>

class WorldObject {
    public:
        // Builder pattern?
        WorldObject(std::string const &path);
        void Draw(Shader &shader);
    private:
        // todo, refactor to add m_ prefix
        Model model;
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 m_scale = glm::vec3(1.0f);
};

#endif
