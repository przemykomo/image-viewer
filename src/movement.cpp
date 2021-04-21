#include "movement.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "draw.h"

using namespace gl;
extern unsigned int shaderProgram;

constexpr glm::vec3 ROTATION_AXIS = glm::vec3(0, 0, -1);

//TODO: vim-like number + arrow moves X times to some direction
glm::mat4 movementMatrix = glm::identity<glm::mat4>();
float rotationAngle{0.0f};
glm::vec2 movement{};

void translateImage(float x, float y, GLFWwindow* window) {
    movement.x += x;
    movement.y += y;
    movementMatrix = glm::rotate(movementMatrix, -rotationAngle, ROTATION_AXIS);
    movementMatrix = glm::translate(movementMatrix, glm::vec3(x, y, 0.0f));
    movementMatrix = glm::rotate(movementMatrix, rotationAngle, ROTATION_AXIS);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "movement"), 1, GL_FALSE, glm::value_ptr(movementMatrix));
    draw(window);
}

void scaleImage(float s, GLFWwindow* window) {
    movementMatrix = glm::rotate(movementMatrix, -rotationAngle, ROTATION_AXIS);
    movementMatrix = glm::translate(movementMatrix, glm::vec3(-movement, 0.0f));
    movementMatrix = glm::scale(movementMatrix, glm::vec3(s, s, 0.0f));
    movementMatrix = glm::translate(movementMatrix, glm::vec3(movement, 0.0f));
    movementMatrix = glm::rotate(movementMatrix, rotationAngle, ROTATION_AXIS);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "movement"), 1, GL_FALSE, glm::value_ptr(movementMatrix));
    draw(window);
}

void rotateImage(float angle, GLFWwindow* window) {
    rotationAngle += angle;
    glm::mat4 rotationMatrix = glm::rotate(glm::identity<glm::mat4>(), rotationAngle, ROTATION_AXIS);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "rotation"), 1, GL_FALSE, glm::value_ptr(rotationMatrix));
    draw(window);
}

void resetMovement(GLFWwindow* window) {
    movementMatrix = glm::identity<glm::mat4>();
    rotationAngle = 0.0f;
    movement = {0.0f, 0.0f};
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "movement"), 1, GL_FALSE, glm::value_ptr(movementMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "rotation"), 1, GL_FALSE, glm::value_ptr(glm::identity<glm::mat4>()));
    draw(window);
}
