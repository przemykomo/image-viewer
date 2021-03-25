#pragma once

#include <glbinding/glbinding.h>

namespace debug {
    void init();
    void glCallback(const glbinding::FunctionCall& call);
    void errorCallback(int error, const char* description);
}
