#include "image.h"
#include "draw.h"
#include "stb_image.h"
#include <filesystem>
#include <iostream>
#include <vector>

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

void nextImage(GLFWwindow *window) {
    int width, height;
    unsigned char* data;

    std::vector<std::string>::size_type previous{ currentFile };

    do {
        ++currentFile;
        if (currentFile == files.size()) {
            currentFile = 0;
        }

        data = loadImage(files[currentFile].c_str(), &width, &height);

        if (currentFile == previous) {
            return;
        }
    } while (data == nullptr);

    storeImageInGPU(data, width, height);
    stbi_image_free(data);
    draw(window);
}

void previousImage(GLFWwindow *window) {
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
    draw(window);
}

