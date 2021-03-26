#include "debug.h"
#include <glbinding/AbstractFunction.h>
#include <glbinding/FunctionCall.h>
#define GLFW_INCLUDE_NONE
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace debug {
    void initGLFW() {
#ifdef DEBUG
        glfwSetErrorCallback(errorCallback);
#endif
    }

    void initOpenGL() {
#ifdef DEBUG
        glbinding::setCallbackMask(glbinding::CallbackMask::After | glbinding::CallbackMask::ParametersAndReturnValue);
        glbinding::setAfterCallback(glCallback);
#endif
    }

    void glCallback(const glbinding::FunctionCall& call) {
#ifdef DEBUG
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

            std::cout << "; glGetError() -> " << (unsigned int)gl::glGetError() << std::endl; 
        }
#endif
    }

    void errorCallback(int error, const char* description) {
#ifdef DEBUG
        std::cerr << description;
#endif
    }
}
