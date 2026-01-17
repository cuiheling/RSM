#define STB_IMAGE_IMPLEMENTATION
#include <Shader.h>
#include <Camera.h>
#include <Model.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <random>
#include <iostream>

using namespace std;

Camera camera;
double deltaTime = 0.0f, lastTime = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.KeyboardMove((float)deltaTime / 4.0f, 'W');
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.KeyboardMove((float)deltaTime / 4.0f, 'A');
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.KeyboardMove((float)deltaTime / 4.0f, 'S');
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.KeyboardMove((float)deltaTime / 4.0f, 'D');
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.KeyboardRotate(0.0f, 4.0f);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.KeyboardRotate(0.0f, -4.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.KeyboardRotate(-4.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.KeyboardRotate(4.0f, 0.0f);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    camera.MouseRotate((float)xpos, (float)ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ScrollScale((float)xoffset, (float)yoffset);
}

unsigned int LoadTexture(const char Address[]) {
    int width, height, nrChannels;
    unsigned char* data1 = stbi_load(Address, &width, &height, &nrChannels, 0);
    unsigned int texture_n;
    if (data1) {
        GLenum format1, format2;
        if (nrChannels == 1) {
            format1 = GL_RED;
            format2 = GL_RED;
        }
        else if (nrChannels == 3) {
            format1 = GL_SRGB;
            format2 = GL_RGB;
        }
        else if (nrChannels == 4) {
            format1 = GL_SRGB_ALPHA;
            format2 = GL_RGBA;
        }
        //std::cout << width << " " << height << " " << nrChannels << std::endl;
        glGenTextures(1, &texture_n);
        glBindTexture(GL_TEXTURE_2D, texture_n);
        glTexImage2D(GL_TEXTURE_2D, 0, format1, width, height, 0, format2, GL_UNSIGNED_BYTE, data1);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data1);
    }
    else {
        std::cout << "Failed to load texture_n" << std::endl;
    }
    return texture_n;
}

void drawGuitar(Shader shader, Model sgb) {
    shader.use();
    glm::mat4 model;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.25f));
    shader.setMatrix4("Model", model);
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    sgb.Draw(shader);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.25f));
    shader.setMatrix4("Model", model);
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    sgb.Draw(shader);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.125f));
    shader.setMatrix4("Model", model);
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    sgb.Draw(shader);
}

void drawBox(Shader shader, unsigned int vao) {
    // floor
    shader.use();
    glm::mat4 model;
    glBindVertexArray(vao);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
    model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
    shader.setMatrix4("Model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(512, 512, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    stbi_set_flip_vertically_on_load(true);
    Model SGB("backpack/backpack.obj");

    float cubeVertices[] = {
        //   positions          normal                 texture Coords
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f
    };
    float screenVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    std::mt19937 gen(12345);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    const float pai = 3.1415926;
    //float sampleX[405], sampleY[405], sampleWt[405];
    /*float samples[1605];
    for (int i = 0; i < 400; i++){
        float s1 = dist(gen), s2 = dist(gen), s3 = dist(gen);
        float th1 = 2 * pai * s2, th2 =  acos(1.0f - 2.0f * s3);
        samples[i * 4] = 0.3 * s1 * cos(th2);
        samples[i * 4 + 1] = 0.3 * s1 * sin(th2) * sin(th1);
        samples[i * 4 + 2] = 0.3 * s1 * sin(th2) * cos(th1);
        samples[i * 4 + 3] = s1 * s1;
    }*/
    float samples[1205];
    for (int i = 0; i < 400; i++){
        float s1 = dist(gen), s2 = dist(gen);
        samples[i * 3] = s1 * sin(s2 * 2 * pai);
        samples[i * 3 + 1] = s1 * cos(s2 * 2 * pai);
        samples[i * 3 + 2] = s1 * s1;
    }

    unsigned int lBuffer;
    glGenFramebuffers(1, &lBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, lBuffer);
    GLuint lDepth;
    glGenTextures(1, &lDepth);
    glBindTexture(GL_TEXTURE_CUBE_MAP, lDepth);
    for (GLuint i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 512, 512, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lDepth, 0);
    unsigned int lPosition;
    glGenTextures(1, &lPosition);
    glBindTexture(GL_TEXTURE_CUBE_MAP, lPosition);
    for (GLuint i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, lPosition, 0);
    unsigned int lNormal;
    glGenTextures(1, &lNormal);
    glBindTexture(GL_TEXTURE_CUBE_MAP, lNormal);
    for (GLuint i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, lNormal, 0);
    unsigned int lFlux;
    glGenTextures(1, &lFlux);
    glBindTexture(GL_TEXTURE_CUBE_MAP, lFlux);
    for (GLuint i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, lFlux, 0);
    /*unsigned int lDepth;
    glGenTextures(1, &lDepth);
    glBindTexture(GL_TEXTURE_2D, lDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 512, 512, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, lDepth, 0);*/

    GLuint Lattachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, Lattachments);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, lBuffer);
    GLenum Lstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (Lstatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "GBuffer incomplete: 0x" << std::hex << Lstatus << std::dec << std::endl;
    }
    else {
        std::cout << "GBuffer COMPLETE" << std::endl;
    }
    GLenum Lerr;
    while ((Lerr = glGetError()) != GL_NO_ERROR) {
        std::cout << "GL Lerror: 0x" << std::hex << Lerr << std::dec << std::endl;
    }

    unsigned int cBuffer;
    glGenFramebuffers(1, &cBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, cBuffer);
    unsigned int cPosition;
    glGenTextures(1, &cPosition);
    glBindTexture(GL_TEXTURE_2D, cPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cPosition, 0);
    unsigned int cNormal;
    glGenTextures(1, &cNormal);
    glBindTexture(GL_TEXTURE_2D, cNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, cNormal, 0);
    GLuint cAlbedoSpec;
    glGenTextures(1, &cAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, cAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, cAlbedoSpec, 0);

    GLuint Cattachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, Cattachments);

    unsigned int depthRBO;
    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, cBuffer);
    GLenum Cstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (Cstatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "GBuffer incomplete: 0x" << std::hex << Cstatus << std::dec << std::endl;
    }
    else {
        std::cout << "GBuffer COMPLETE" << std::endl;
    }
    GLenum Cerr;
    while ((Cerr = glGetError()) != GL_NO_ERROR) {
        std::cout << "GL error: 0x" << std::hex << Cerr << std::dec << std::endl;
    }

    unsigned int lrBuffer;
    glGenFramebuffers(1, &lrBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, lrBuffer);
    unsigned int lowresBuffer;
    glGenTextures(1, &lowresBuffer);
    glBindTexture(GL_TEXTURE_2D, lowresBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lowresBuffer, 0);
    unsigned int lrRBO;
    glGenRenderbuffers(1, &lrRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, lrRBO); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 128, 128);  
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);
    // screen VAO
    unsigned int screenVAO, screenVBO;
    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);
    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    GLuint sampleSSBO;
    glGenBuffers(1, &sampleSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sampleSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec3) * 400, samples, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, sampleSSBO);

    Shader depthShader("depth.vs", "depth.gs", "depth.fs");
    Shader bufferShader("buffer.vs", "buffer.fs");
    Shader debugShader("debug.vs", "debug.fs");
    Shader screenShader("screen.vs", "screen.fs");
    Shader lowresShader("lowres.vs", "lowres.fs");
    Shader lampShader("lamp.vs", "lamp.fs");

    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f), lightCol(1.0f, 1.0f, 1.0f);
    glm::mat4 lprojection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 20.0f);
    //glm::mat4 lprojection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.50f);
    std::vector<glm::mat4> lviews;
    lviews.push_back(glm::lookAt(lightPos, lightPos + glm::vec3(1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0)));
    lviews.push_back(glm::lookAt(lightPos, lightPos + glm::vec3(-1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0)));
    lviews.push_back(glm::lookAt(lightPos, lightPos + glm::vec3(0.0,1.0,0.0), glm::vec3(0.0,0.0,1.0)));
    lviews.push_back(glm::lookAt(lightPos, lightPos + glm::vec3(0.0,-1.0,0.0), glm::vec3(0.0,0.0,-1.0)));
    lviews.push_back(glm::lookAt(lightPos, lightPos + glm::vec3(0.0,0.0,1.0), glm::vec3(0.0,-1.0,0.0)));
    lviews.push_back(glm::lookAt(lightPos, lightPos + glm::vec3(0.0,0.0,-1.0), glm::vec3(0.0,-1.0,0.0)));
    depthShader.use();
    for (GLuint i = 0; i < 6; i++) {
        char str[20] = "lightViews[i]";
        str[11] = i + '0';
        printf("%s\n", str);
        depthShader.setMatrix4(str, lviews[i]);
    }
    depthShader.setMatrix4("lightProj", lprojection);
    depthShader.setVector2("rsmResolution", glm::vec2(512.0f, 512.0f));
    depthShader.setFloat("fovX", 90.0f);
    depthShader.setFloat("fovY", 90.0f);
    depthShader.setFloat("far_plane", 20.0f);
    depthShader.setVector3("lightCol", lightCol);
    depthShader.setVector3("lightPos", lightPos);
    depthShader.setInt("albedoMap", 0);
    bufferShader.use();
    bufferShader.setInt("material.texture_diffuse1", 0);
    bufferShader.setInt("material.texture_specular1", 1);
    screenShader.use();
    screenShader.setFloat("shininess", 32.0f);
    screenShader.setVector3("light.ambient", 0.2f, 0.2f, 0.2f);
    screenShader.setVector3("light.diffuse", 0.5f, 0.5f, 0.5f);
    screenShader.setVector3("light.specular", 1.0f, 1.0f, 1.0f);
    screenShader.setFloat("far_plane", 20.0f);
    screenShader.setInt("cbuffer.cPosition", 0);
    screenShader.setInt("cbuffer.cNormal", 1);
    screenShader.setInt("cbuffer.cAlbedoSpec", 2);
    screenShader.setInt("shadowMap", 3);
    screenShader.setInt("lbuffer.lPosition", 4);
    screenShader.setInt("lbuffer.lNormal", 5);
    screenShader.setInt("lbuffer.lFlux", 6);
    screenShader.setInt("lowresMap", 7);
    screenShader.setVector3("light.position", lightPos);
    screenShader.setVector3("light.color", lightCol);
    lowresShader.use();
    lowresShader.setVector3("lightPos", lightPos);
    lowresShader.setInt("cbuffer.cPosition", 0);
    lowresShader.setInt("cbuffer.cNormal", 1);
    lowresShader.setInt("lbuffer.lPosition", 2);
    lowresShader.setInt("lbuffer.lNormal", 3);
    lowresShader.setInt("lbuffer.lFlux", 4);
    debugShader.use();
    debugShader.setInt("depthMap", 0);
    debugShader.setInt("normMap", 1);

    unsigned int diffuseTex = LoadTexture("metal.png");
    unsigned int specularTex = LoadTexture("newspec.png");

    glViewport(0, 0, 512, 512);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        double currentTime = (double)glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glm::vec3 camPos = camera.cameraPos;
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.ComputeView();
        glm::mat4 projection = camera.ComputeProjection0();

        glViewport(0, 0, 512, 512);
        glBindFramebuffer(GL_FRAMEBUFFER, lBuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        depthShader.use();
        depthShader.setInt("Reverse", 0);
        depthShader.setInt("useTex", 1);
        drawGuitar(depthShader, SGB);
        depthShader.use();
        depthShader.setInt("Reverse", 1);
        depthShader.setInt("useTex", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTex);
        drawBox(depthShader, cubeVAO);
        glBindVertexArray(0);

        glViewport(0, 0, 512, 512);
        glBindFramebuffer(GL_FRAMEBUFFER, cBuffer);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        //glDepthMask(GL_TRUE);
        bufferShader.use();
        bufferShader.setMatrix4("View", view);
        bufferShader.setMatrix4("Proj", projection);
        bufferShader.setInt("Reverse", 0);
        bufferShader.setInt("useTex", 1);
        drawGuitar(bufferShader, SGB);
        bufferShader.setInt("Reverse", 1);
        bufferShader.setInt("useTex", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularTex);
        drawBox(bufferShader, cubeVAO);

        /*glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, 512, 512);
        glm::mat4 Bview = glm::mat4(glm::mat3(view));
        debugShader.use();
        debugShader.setMatrix4("View", Bview);
        debugShader.setMatrix4("Proj", projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, lDepth);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, lNormal);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);*/
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glViewport(0, 0, 128, 128);
        lowresShader.use();
        glBindFramebuffer(GL_FRAMEBUFFER, lrBuffer);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, lPosition);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, lNormal);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, lFlux);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, sampleSSBO);
        glBindVertexArray(screenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, 512, 512);
        screenShader.use();
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, cAlbedoSpec);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, lDepth);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, lPosition);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, lNormal);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, lFlux);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, lowresBuffer);
        screenShader.setVector3("viewPos", camPos);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, sampleSSBO);
        glBindVertexArray(screenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, cBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, 512, 512, 0, 0, 512, 512, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        lampShader.use();
        glBindVertexArray(cubeVAO);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        lampShader.setMatrix4("Model", model);
        lampShader.setMatrix4("View", view);
        lampShader.setMatrix4("Proj", projection);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}