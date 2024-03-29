#include "Resource.hpp"

Image ResourceManager::load_image_from_path(const std::string &path) {
    Image res;
    unsigned char *data = stbi_load(path.c_str(), (int *) &res.width, (int *) &res.height, (int *) &res.no_components, 0);
    if (!data) {
        stbi_image_free(data);
        data = nullptr; /* later check if (data) ... */
    }
    res.data = data;
    return res;
}

Texture ResourceManager::load_ogl_texture_from_path(const std::string &path, const Texture::TextureType type, bool srgb, bool array) {
    Image res = load_image_from_path(path);
    Texture tex;

    GLenum internalFormat;
    GLenum dataFormat;

    if (res.no_components == 1) {
        internalFormat = dataFormat = GL_RED;
    } else if (res.no_components == 3) {
        internalFormat = srgb ? GL_SRGB : GL_RGB;
        dataFormat     = GL_RGB;
    } else if (res.no_components == 4) {
        internalFormat = srgb ? GL_SRGB_ALPHA : GL_RGBA;
        dataFormat     = GL_RGBA;
    }

    tex.array = array;

    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, res.width, res.height, 0, dataFormat, GL_UNSIGNED_BYTE, res.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(res.data);

    switch (type) {
        case Texture::TextureType::DIFFUSE:
            tex.type = "material.texture_diffuse";
            break;
        case Texture::TextureType::SPECULAR:
            tex.type = "material.texture_specular";
            break;
        case Texture::TextureType::EMISSION:
            tex.type = "material.texture_emission";
            break;
        case Texture::TextureType::NORMAL:
            tex.type = "material.texture_normal";
            break;
        case Texture::TextureType::HEIGHT:
            tex.type = "material.texture_height";
            break;
    }

    return tex;
}


const char *ResourceManager::load_file(const std::string &path) {
    std::ifstream stream;
    stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    const char *content;
    try {
        stream.open(path);
        std::stringstream _content;
        _content << stream.rdbuf();
        stream.close();
        content = _content.str().c_str();
    } catch (const std::ifstream::failure &e) {
        std::cout << "File Load Error: " << e.what() << std::endl;
        return nullptr;
    }
    return content;
}