#include "debug.h"
#include "shaders.h"
#include "draw.h"
#include "movement.h"
#include "callbacks.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
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
extern glm::mat4 movementMatrix;

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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    const GLFWvidmode* vidMode{ glfwGetVideoMode(glfwGetPrimaryMonitor()) };
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
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "rotation"), 1, GL_FALSE, glm::value_ptr(glm::identity<glm::mat4>()));
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

