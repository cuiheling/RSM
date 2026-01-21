cmake_minimum_required(VERSION 3.15)
project(OpenGLDemo)

set(CMAKE_CXX_STANDARD 17)

# Build third-party libs from source to avoid MSVC-only prebuilt .lib/.dll.a issues
set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../assimp ${CMAKE_BINARY_DIR}/assimp_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../glfw-3.4 ${CMAKE_BINARY_DIR}/glfw_build)

# ---- Dear ImGui ----
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../imgui)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/../../glad/include
)

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
# imgui end


add_executable(OpenGLDemo
    src/main.cpp
    ../../glad/src/glad.c
)

find_package(OpenGL REQUIRED)

target_link_libraries(OpenGLDemo PRIVATE
    imgui
    glfw            # built from glfw-3.4 source
    assimp::assimp  # built from assimp source
    OpenGL::GL
)

target_include_directories(OpenGLDemo PRIVATE
    ../../Source/Include
    ../../glad/include
)

# Copy runtime resources (shaders, textures, model) next to the built executable
set(RSM_RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(RSM_RESOURCE_FILES
    buffer.vs buffer.fs
    depth.vs depth.fs depth.gs
    lowres.vs lowres.fs
    screen.vs screen.fs
    debug.vs debug.fs
    lamp.vs lamp.fs
    metal.png newspec.png
)
foreach(f ${RSM_RESOURCE_FILES})
    configure_file(${RSM_RESOURCE_DIR}/${f} ${CMAKE_BINARY_DIR}/${f} COPYONLY)
endforeach()
configure_file(${RSM_RESOURCE_DIR}/backpack/backpack.obj ${CMAKE_BINARY_DIR}/backpack/backpack.obj COPYONLY)
configure_file(${RSM_RESOURCE_DIR}/backpack/backpack.mtl ${CMAKE_BINARY_DIR}/backpack/backpack.mtl COPYONLY)
configure_file(${RSM_RESOURCE_DIR}/backpack/backpack.fbx ${CMAKE_BINARY_DIR}/backpack/backpack.fbx COPYONLY)
file(COPY ${RSM_RESOURCE_DIR}/backpack/ DESTINATION ${CMAKE_BINARY_DIR}/backpack/)
