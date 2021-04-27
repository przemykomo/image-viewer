#pragma once

#define GLFW_INCLUDE_NONE
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>

unsigned char* loadImage(const char* fileName, int* width, int* height);
inline void storeImageInGPU(unsigned char* data, int width, int height) {
    using namespace gl;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}
void initDirectory(std::string_view dir);
unsigned char* nextImage(int* width, int* height);
void nextImageDraw(GLFWwindow* window);
void previousImageDraw(GLFWwindow* window);
void updateProjectionMatrix(int width, int height);
