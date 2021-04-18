#include "shaders.h"
#include <glbinding/gl/gl.h>
#include <iostream>


namespace shaders {
    using namespace gl;

    const char *vertexShaderSource{R"(
        #version 330 core
        layout (location = 0) in vec2 pos;
        layout (location = 1) in vec2 inTexCoord;

        out vec2 TexCoord;

        uniform mat4 projection;
        uniform mat4 movement;
        uniform mat4 rotation;

        void main() {
           gl_Position = rotation * movement * projection * vec4(pos, 0.0f, 1.0f);
           TexCoord = inTexCoord;
        }
    )"};

    const char *fragmentShaderSource{R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D image;

        void main() {
           FragColor = texture(image, TexCoord);
        }
    )"};


    const float vertices[] = {
        // position   // texture coords
         1.0f,  1.0f, 1.0f, 1.0f, // top right
         1.0f, -1.0f, 1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f, 1.0f  // top left
    };

    const unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    unsigned int createShader(GLenum type, const GLchar* const* source) {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, source, NULL);
        glCompileShader(shader);

#ifdef DEBUG
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "SHADER LOG:\n" << infoLog << std::endl;
#endif
        return shader;
    }

    unsigned int createProgram() {
        unsigned int vertexShader = createShader(GL_VERTEX_SHADER, &vertexShaderSource);
        unsigned int fragmentShader = createShader(GL_FRAGMENT_SHADER, &fragmentShaderSource);

        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

#ifdef DEBUG
        // check for linking errors
        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "SHADER PROGRAM LOG:\n" << infoLog << std::endl;
#endif

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    }

    void createBuffers(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO) {
        glGenVertexArrays(1, VAO);
        glGenBuffers(1, VBO);
        glGenBuffers(1, EBO);
        glBindVertexArray(*VAO);
        glBindBuffer(GL_ARRAY_BUFFER, *VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void createTexture(unsigned int shaderProgram) {
        unsigned int texture;
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "image"), 0);
    }
}
