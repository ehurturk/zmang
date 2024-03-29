<VERTEX>
#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

out vec2 uv_coord;

void main() {
    uv_coord    = uv;
    gl_Position = vec4(pos, 0.0f, 1.0f);
}
<FRAGMENT>
#version 330 core

out vec4 FragCol;

uniform sampler2D tex;

in vec2 uv_coord;

void main() {
    FragCol = vec4(texture(tex, uv_coord).rgb, 1.0f);
    // float depthValue = texture(tex, uv_coord).r;
    // FragCol = vec4(vec3(depthValue), 1.0);
}