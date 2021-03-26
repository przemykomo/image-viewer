cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(piv VERSION 1.0)

file(GLOB_RECURSE SRC_FILES src/*.cpp CMAKE_CONFIGURE_DEPENDS)
add_executable(piv ${SRC_FILES})

target_include_directories(piv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

find_package(glbinding REQUIRED)

target_link_libraries(piv
    ${GLFW_LIBRARIES}  
    glbinding::glbinding
)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    target_link_libraries(piv glbinding::glbinding-aux)
    add_compile_definitions(DEBUG)
endif()
