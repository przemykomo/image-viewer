#include "debug.h"
#include <glbinding/AbstractFunction.h>
#include <glbinding/FunctionCall.h>
#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>
#include <iostream>

namespace debug {
    void init() {
        glbinding::setCallbackMask(glbinding::CallbackMask::After | glbinding::CallbackMask::ParametersAndReturnValue);
        glbinding::setAfterCallback(glCallback);
    }

    void glCallback(const glbinding::FunctionCall& call) {
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
    }

    void errorCallback(int error, const char* description) {
        std::cerr << description;
    }
}
