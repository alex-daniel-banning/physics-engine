#ifndef MESH_H
#define MESH_H
#include <vector>
#include "Vertex.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

class Mesh {
    public:
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture>      textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        void Draw(Shader &Shader);
    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh();
};

#endif
