#include "callbacks.h"
#include "movement.h"
#include "draw.h"
#include "image.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl;

extern unsigned int shaderProgram;
extern float imageAspectRatio;

void setupCallbacks(GLFWwindow* window) {
    glfwSetKeyCallback(window, keyCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetWindowRefreshCallback(window, windowRefreshCallback);
}

void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (mods & GLFW_MOD_ALT) {
            switch (key) {
                case GLFW_KEY_RIGHT:
                case GLFW_KEY_L:
                    nextImageDraw(window);
                    break;
                case GLFW_KEY_LEFT:
                case GLFW_KEY_H:
                    previousImageDraw(window);
                    break;
            }
        } else if (mods & GLFW_MOD_SHIFT) {
            switch (key) {
                case GLFW_KEY_UP:
                case GLFW_KEY_K:
                    scaleImage(2.0f, window);
                    break;
                case GLFW_KEY_DOWN:
                case GLFW_KEY_J:
                    scaleImage(0.5f, window);
                    break;
                case GLFW_KEY_RIGHT:
                case GLFW_KEY_L:
                    rotateImage(glm::radians(10.0f), window);
                    break;
                case GLFW_KEY_LEFT:
                case GLFW_KEY_H:
                    rotateImage(glm::radians(-10.0f), window);
                    break;
            }
        } else {
            switch (key) {
                case GLFW_KEY_ESCAPE:
                case GLFW_KEY_Q:
                    glfwSetWindowShouldClose(window, true);
                    break;
                case GLFW_KEY_RIGHT:
                case GLFW_KEY_L:
                    translateImage(-0.05f, 0.0f, window);
                    break;
                case GLFW_KEY_LEFT:
                case GLFW_KEY_H:
                    translateImage(0.05f, 0.0f, window);
                    break;
                case GLFW_KEY_UP:
                case GLFW_KEY_K:
                    translateImage(0.0f, -0.05f, window);
                    break;
                case GLFW_KEY_DOWN:
                case GLFW_KEY_J:
                    translateImage(0.0f, 0.05f, window);
                    break;
                case GLFW_KEY_R:
                    resetMovement(window);
                    break;
            }
        }
    }
}

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    updateProjectionMatrix(width, height);
    draw(window);
}

void windowRefreshCallback(GLFWwindow* window) {
    draw(window);
}
