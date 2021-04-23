#pragma once

#define GLFW_INCLUDE_NONE
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>

void setupCallbacks(GLFWwindow* window);
void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);
void windowSizeCallback(GLFWwindow* window, int width, int height);
void windowRefreshCallback(GLFWwindow* window);
