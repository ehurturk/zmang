#include "Model.hpp"
#include <string>

unsigned int TextureFromFile(const std::string &path, const std::string &directory, bool gamma = false);


Model::Model(const std::string &path) {
    load_model(path);
}

Model::~Model() {
    for (const auto &mesh : meshes) {
        mesh.destroy();
    }
}

void Model::draw(const Shader &shader) {
    for (const auto &mesh : meshes)
        mesh.draw(shader);
}

void Model::load_model(const std::string &path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of("/"));
    std::cout << "load_model(" << path << ")" << std::endl;
    process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode *node, const aiScene *scene) {
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene);
    }
}

Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<Texture> textures;
    std::vector<ui32> indices;

    // fill in vertex data form the model info
    for (int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 temp_vec;
        // get the position data
        temp_vec.x      = mesh->mVertices[i].x;
        temp_vec.y      = mesh->mVertices[i].y;
        temp_vec.z      = mesh->mVertices[i].z;
        vertex.position = temp_vec;
        // get the normal data
        temp_vec.x     = mesh->mNormals[i].x;
        temp_vec.y     = mesh->mNormals[i].y;
        temp_vec.z     = mesh->mNormals[i].z;
        vertex.normals = temp_vec;
        // get the uv data
        if (mesh->mTextureCoords[0]) {
            glm::vec2 uv_data;
            uv_data.x = mesh->mTextureCoords[0][i].x;
            uv_data.y = mesh->mTextureCoords[0][i].y;
            vertex.uv = uv_data;
        } else
            vertex.uv = glm::vec2(0.0f, 0.0f);
        vertices.push_back(vertex);
    }
    for (int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *mat                   = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuse_maps = load_material_textures(mat, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
        std::vector<Texture> specular_maps = load_material_textures(mat, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
        std::vector<Texture> emission_maps = load_material_textures(mat, aiTextureType_EMISSIVE, "texture_emission");
        textures.insert(textures.end(), emission_maps.begin(), emission_maps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::load_material_textures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    for (int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (int j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture;
            texture.id   = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

unsigned int TextureFromFile(const std::string &path, const std::string &directory, bool gamma) {
    std::string filename = path;
    filename             = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}