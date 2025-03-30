#ifndef MODEL_H
#define MODEL_H
#include <Shader.hpp>
#include <model/Mesh.hpp>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model {
    public:
        Model(char *path) {
            loadModel(path);
        }
        void Draw(Shader &shader);
    private:
        // model data
        std::vector<Mesh> meshes;
        std::string directory;
        std::vector<Texture> textures_loaded;

        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                std::string typeName);
};

#endif
