#include "image.h"
#include "draw.h"
#include "stb_image.h"
#include <filesystem>
#include <iostream>
#include <vector>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl;

extern unsigned int shaderProgram;
float imageAspectRatio{1.0f}; // height / width
unsigned char* loadImage(const char* fileName, int* width, int* height) {
    int comp;
    unsigned char* data = stbi_load(fileName, width, height, &comp, STBI_rgb_alpha);

    if (data != nullptr) {
        imageAspectRatio = (float)*height / *width;
    }

    return data;
}

std::vector<std::string> files{};
std::vector<std::string>::size_type currentFile{};

void initDirectory(std::string_view dir) {
    for (auto& p : std::filesystem::directory_iterator(dir)) {
        files.push_back(p.path().string());
    }
    files.shrink_to_fit();
}

unsigned char* nextImage(int* width, int* height) {
    unsigned char* data;

    std::vector<std::string>::size_type previous{ currentFile };

    do {
        ++currentFile;
        if (currentFile == files.size()) {
            currentFile = 0;
        }

        data = loadImage(files[currentFile].c_str(), width, height);

        if (currentFile == previous) {
            return nullptr;
        }
    } while (data == nullptr);

    return data;
}

void nextImageDraw(GLFWwindow *window) {
    int width, height;
    unsigned char* data{ nextImage(&width, &height) };

    if (data != nullptr) {
        storeImageInGPU(data, width, height);
        stbi_image_free(data);
        int winWidth, winHeight;
        glfwGetWindowSize(window, &winWidth, &winHeight);
        updateProjectionMatrix(winWidth, winHeight);
        draw(window);
    }
}

void previousImageDraw(GLFWwindow *window) {
    int width, height;
    unsigned char* data;

    std::vector<std::string>::size_type previous{ currentFile };

    do {
        if (currentFile == 0) {
            currentFile = files.size();
        }

        --currentFile;

        data = loadImage(files[currentFile].c_str(), &width, &height);

    } while (data == nullptr);


    storeImageInGPU(data, width, height);
    stbi_image_free(data);
    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);
    updateProjectionMatrix(winWidth, winHeight);
    draw(window);
}

void updateProjectionMatrix(int width, int height) {
    glm::mat4 projection = glm::identity<glm::mat4>();
    if (height > width * imageAspectRatio) {
        projection = glm::scale(projection, glm::vec3(1, width * imageAspectRatio / height, 1));
    } else {
        projection = glm::scale(projection, glm::vec3(height / (imageAspectRatio * width), 1, 1));
    }

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}
