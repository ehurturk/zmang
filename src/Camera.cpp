#include "Camera.h"

void Camera3D::mouse_callback_fn(double xpos, double ypos) {
    if (glfwGetMouseButton(window->get_raw_window(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        return;
    }

    if (firstTimeEntered) {
        lastX            = xpos;
        lastY            = ypos;
        firstTimeEntered = false;
    }

    float offsetX = xpos - lastX;
    float offsetY = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    offsetX *= sensitivity;
    offsetY *= sensitivity;

    YAW += offsetX;
    PITCH += offsetY;

    if (PITCH > 89.0f)
        PITCH = 89.0f;
    if (PITCH < -89.0f)
        PITCH = -89.0f;

    update_cam_dirs();
}

void Camera3D::key_callback_fn(int key, int scan_code, int action, int mods) {
}

void Camera3D::adjust_viewport(int width, int height) {
    projection = glm::perspective(glm::radians(FOV), (float) width / (float) height, 0.1f, 100.0f);
    glViewport(0, 0, width, height);
}


Camera3D::Camera3D(std::unique_ptr<Window> &window) : window(window), position(glm::vec3(0.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)), right(glm::vec3(0.0f)), front(glm::vec3(0.0f, 0.0f, -1.0f)), lastX((float) window->config.width / 2.0f), lastY((float) window->config.height / 2.0f), projection(glm::perspective(glm::radians(FOV), (float) window->config.width / window->config.height, 0.1f, 100.0f)) {
    update_cam_dirs();
}

void Camera3D::update() {
    view = glm::lookAt(position, position + front, up);
}

void Camera3D::update_cam_dirs() {
    glm::vec3 direction;
    direction.x = glm::cos(glm::radians(YAW)) * glm::cos(glm::radians(PITCH));
    direction.y = glm::sin(glm::radians(PITCH));
    direction.z = glm::sin(glm::radians(YAW)) * glm::cos(glm::radians(PITCH));

    front = glm::normalize(direction);
}

glm::mat4 &Camera3D::get_projection_matrix() { return projection; }
glm::mat4 &Camera3D::get_view_matrix() { return view; }
