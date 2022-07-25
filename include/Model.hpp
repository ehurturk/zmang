#pragma once

#include "common.h"
#include "Shader.hpp"
#include "Resource.hpp"
#include "Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
public:
    explicit Model(const std::string &path);
    ~Model();
    void draw(const Shader &shader);

private:
    void load_model(const std::string &path);
    void process_node(aiNode *node, const aiScene *scene);
    Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type,
                                                std::string typeName);

    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;
};