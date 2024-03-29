#pragma once

#include "Engine.hpp"
#include "Quark.hpp"
#include "glm/fwd.hpp"
#include "graphics/SkyBox.h"

class NormalMap : public Application {
public:
    NormalMap();
    ~NormalMap();

    // Executed on initialization
    void start() override;
    // Executed each frame
    void update(float dt) override;
    // Executed each frame for UI
    void imgui_update() override;

    // Additional functions...
    void process_input(float dt);

private:
    // All private variables...
    Shader normal_shader;
    Shader light_source_shader;
    SkyBox *box;
    Model car;
    Model light;

    bool normal_enabled = true;
    bool draw_lights    = false;

#define COLOR_WHITE glm::vec3(1.0f)
#define COLOR_BLUE  glm::vec3(0.0f, 0.0f, 1.0f)
#define COLOR_GREEN glm::vec3(0.0f, 1.0f, 0.0f)
#define COLOR_RED   glm::vec3(1.0f, 0.0f, 0.0f)

    glm::vec3 light_pos[4] = {
        glm::vec3(-3.0f, 0.5f, 0.0f),
        glm::vec3(-1.0f, 0.5f, 0.0f),
        glm::vec3(1.0f, 0.5f, 0.0f),
        glm::vec3(3.0f, 0.5f, 0.0f)
    };
    glm::vec3 light_color[4] = { glm::vec3(0.25),
                                 glm::vec3(0.50),
                                 glm::vec3(0.75),
                                 glm::vec3(1.00) };
};
