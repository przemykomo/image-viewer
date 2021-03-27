#include "debug.h"
#include "shaders.h"
#define GLFW_INCLUDE_NONE
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace gl;

unsigned int shaderProgram{0};
float imageAspectRatio{1.0f}; // height / width

void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

void draw(GLFWwindow* window);

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    if (height > width * imageAspectRatio) {
        glViewport(0, 0, width, width * imageAspectRatio);
    } else {
        glViewport(0, 0, height / imageAspectRatio, height);
    }

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

    shaderProgram = shaders::createProgram();
    
    unsigned int VAO, VBO, EBO;
    shaders::createBuffers(&VAO, &VBO, &EBO);

    shaders::createTexture(shaderProgram);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

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
