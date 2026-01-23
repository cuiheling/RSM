cmake_minimum_required(VERSION 3.15)
project(OpenGLDemo)

set(CMAKE_CXX_STANDARD 17)

add_executable(OpenGLDemo
    src/main.cpp
    ../../glad/src/glad.c
)

# GLFW
target_link_directories(OpenGLDemo PRIVATE ../../Source/Libs/Debug)

# ---- Dear ImGui ----
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../Source/Include/imgui)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/../../glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Source/Include
)

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

target_link_libraries(imgui PUBLIC glfw3 OpenGL::GL)
# imgui end

find_package(OpenGL REQUIRED)
target_link_libraries(OpenGLDemo PRIVATE glfw3 OpenGL::GL assimp-vc143-mtd imgui)

target_include_directories(OpenGLDemo PRIVATE
    ../../Source/Include
)
