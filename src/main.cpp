#define GLFW_INCLUDE_NONE
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glbinding-aux/debug.h>
#include <glbinding/FunctionCall.h>
#include <glbinding/AbstractFunction.h>

using namespace gl;

void errorCallback(int error, const char* description) {
    std::cerr << description;
}

void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

void draw(GLFWwindow* window);

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    draw(window);
}

void windowRefreshCallback(GLFWwindow* window) {
    draw(window);
}

unsigned int shaderProgram{0};

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 pos;\n"
    "layout (location = 1) in vec2 inTexCoord;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(pos, 0.0f, 1.0f);\n"
    "   TexCoord = inTexCoord;\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D ourTexture;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(ourTexture, TexCoord);\n"
    "}\n\0";

int main() {
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        std::cerr << "Cannot initialize GLFW!\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(640, 480, "Image viewer", NULL, NULL);

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
    
    glbinding::setCallbackMask(glbinding::CallbackMask::After | glbinding::CallbackMask::ParametersAndReturnValue);
    glbinding::setAfterCallback([](const glbinding::FunctionCall & call)
            {
                if (std::string_view(call.function->name()).compare("glGetError")) {
                    std::cout << call.function->name() << "(";
                    for (unsigned i = 0; i < call.parameters.size(); ++i) {
                        std::cout << call.parameters[i].get();
                        if (i < call.parameters.size() - 1) {
                            std::cout << ", ";
                        }
                    }
                    std::cout << ")";

                    if (call.returnValue) {
                        std::cout << " -> " << call.returnValue.get();
                    }

                    std::cout << "; glGetError() -> " << (unsigned int)glGetError() << std::endl; 
                }
            });

    glClearColor(0.0f, 1.0f, 0.7f, 1.0f);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "VERTEX SHADER LOG:\n" << infoLog << std::endl;

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "FRAGMENT SHADER LOG:\n" << infoLog << std::endl;
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "SHADER PROGRAM LOG:\n" << infoLog << std::endl;
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    float vertices[] = {
        // position         // texture coords
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("./container.jpg", &width, &height, &nrChannels, 0);
    if (data == nullptr) {
        std::cerr << "Cannot load texture!\n";
    } else {
        std::cout << "Texture width: " << width << ", height: " << height << '\n';
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
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
