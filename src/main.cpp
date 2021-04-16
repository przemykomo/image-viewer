#include "debug.h"
#include "shaders.h"
#include <glm/ext/matrix_float4x4.hpp>
#define GLFW_INCLUDE_NONE
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl;

unsigned int shaderProgram{0};
float imageAspectRatio{1.0f}; // height / width
//TODO: vim-like number + arrow moves X times to some direction
glm::mat4 movementMatrix = glm::identity<glm::mat4>();

void draw(GLFWwindow* window);

void translateImage(float x, float y, GLFWwindow* window) {
    movementMatrix = glm::translate(movementMatrix, glm::vec3(x, y, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "movement"), 1, GL_FALSE, glm::value_ptr(movementMatrix));
    draw(window);
}

void scaleImage(float s, GLFWwindow* window) {
    movementMatrix = glm::scale(movementMatrix, glm::vec3(s, s, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "movement"), 1, GL_FALSE, glm::value_ptr(movementMatrix));
    draw(window);
}

void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (mods & 1) {
            switch (key) {
                case GLFW_KEY_UP:
                case GLFW_KEY_K:
                    scaleImage(2.0f, window);
                    break;
                case GLFW_KEY_DOWN:
                case GLFW_KEY_J:
                    scaleImage(0.5f, window);
                    break;
            }
        } else {
            switch (key) {
                case GLFW_KEY_ESCAPE:
                case GLFW_KEY_Q:
                    glfwSetWindowShouldClose(window, 1);
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
                    movementMatrix = glm::identity<glm::mat4>();
                    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "movement"), 1, GL_FALSE, glm::value_ptr(movementMatrix));
                    draw(window);
                    break;
            }
        }
    }
}

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    glm::mat4 projection = glm::identity<glm::mat4>();
    if (height > width * imageAspectRatio) {
        projection = glm::scale(projection, glm::vec3(1, width * imageAspectRatio / height, 1));
    } else {
        projection = glm::scale(projection, glm::vec3(height / (imageAspectRatio * width), 1, 1));
    }

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    draw(window);
}

void windowRefreshCallback(GLFWwindow* window) {
    draw(window);
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        return 0;
    }

    stbi_set_flip_vertically_on_load(true);
    int width, height, comp;
    unsigned char* data = stbi_load(argv[argc - 1], &width, &height, &comp, STBI_rgb_alpha);
    if (data == nullptr) {
        std::cerr << "Cannot load texture!\n";
        return 1;
    }

    imageAspectRatio = (float)height / width;

    debug::initGLFW();

    if (!glfwInit()) {
        std::cerr << "Cannot initialize GLFW!\n";
        return 1;
    }

    const GLFWvidmode* vidMode{ glfwGetVideoMode(glfwGetPrimaryMonitor()) };

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(width * vidMode->height / 1080 * 480 / 1080, height * vidMode->height / 1080 * 480 / 1080, "Image viewer", NULL, NULL);

    if (!window) {
        glfwTerminate();
        std::cerr << "Cannot create window!\n";
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetWindowRefreshCallback(window, windowRefreshCallback);
    glfwSwapInterval(0);

    glbinding::initialize(glfwGetProcAddress);
    debug::initOpenGL();

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shaderProgram = shaders::createProgram();
    
    unsigned int VAO, VBO, EBO;
    shaders::createBuffers(&VAO, &VBO, &EBO);

    shaders::createTexture(shaderProgram);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "movement"), 1, GL_FALSE, glm::value_ptr(movementMatrix));
    draw(window);

    while (!glfwWindowShouldClose(window)) {
        glfwWaitEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}

void draw(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
}
