#pragma once

#define GLFW_INCLUDE_NONE
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>

void translateImage(float x, float y, GLFWwindow* window);
void scaleImage(float s, GLFWwindow* window);
void rotateImage(float angle, GLFWwindow* window);
void resetMovement(GLFWwindow* window);
