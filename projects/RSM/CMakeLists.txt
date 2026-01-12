cmake_minimum_required(VERSION 3.15)
project(OpenGLDemo)

set(CMAKE_CXX_STANDARD 17)

add_executable(OpenGLDemo
    src/main.cpp
    ../../glad/src/glad.c
)

# GLFW
target_link_directories(OpenGLDemo PRIVATE ../../Source/Libs/Debug)

find_package(OpenGL REQUIRED)
target_link_libraries(OpenGLDemo PRIVATE glfw3 OpenGL::GL assimp-vc143-mtd)


target_include_directories(OpenGLDemo PRIVATE
    ../../Source/Include
)
