#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/shader.h"
#include "include/model.h"
#include "include/light.h"

std::vector<glm::vec3> lightPositions;
std::vector<glm::vec3> lightColors;
Light lights(5);

int mode = 0;

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        lights.remove();
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        lights.add(lightPositions, lightColors);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        mode = 0;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        mode = 1;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        mode = 2;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        mode = 3;
}

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
const glm::vec3 landscapePos = glm::vec3(-3.0, -10.0, -3.0);
const float quadVertices[] = {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

const glm::vec3 positionCamera = glm::vec3(-3.0f, 0.0f, 17.0f);

void initialization() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_STENCIL_BITS, 4);
    glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

GLFWwindow *createWindow() {
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return nullptr;
    }
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    return window;
}

unsigned int quadVAO;
unsigned int quadVBO;

void initRenderScene() {
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
}

unsigned int gBuffer;
unsigned int gPosition, gNormal, gAlbedoSpec;

void initGBuffer() {
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                        100.0f);
glm::mat4 view = glm::lookAt(positionCamera, positionCamera +
                                             glm::vec3(cos(glm::radians(-90.0f)) * cos(glm::radians(-25.0f)),
                                                       sin(glm::radians(-25.0f)),
                                                       sin(glm::radians(-90.0f)) * cos(glm::radians(-25.0f))),
                             glm::vec3(0.0f, 1.0f, 0.0f));
int main() {
    initialization();
    GLFWwindow *window = createWindow();
    initRenderScene();
    Shader shaderGeometryPass("../shaders/g_buffer_vs.glsl", "../shaders/g_buffer_fs.glsl");
    Shader shaderLightingPass("../shaders/deferred_shading_vs.glsl", "../shaders/deferred_shading_fs.glsl");
    Shader shaderLightBox("../shaders/light_sphere_vs.glsl", "../shaders/light_sphere_fs.glsl");
    Model sphere("../sphere/sphere.obj");
    Model landscape("../landscape/landscape.obj");
    initGBuffer();
    const unsigned int NR_LIGHTS = 5;
    srand(13);
    for (unsigned int i = 0; i < NR_LIGHTS; i++) {
        float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        float yPos = -6.0;
        float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
        float rColor = ((rand() % 100) / 200.0f) + 0.5;
        float gColor = ((rand() % 100) / 200.0f) + 0.5;
        float bColor = ((rand() % 100) / 200.0f) + 0.5;
        lightColors.push_back(glm::vec3(rColor, gColor, bColor));
    }
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedoSpec", 2);
    shaderLightingPass.setInt("mode", mode);
    shaderLightingPass.setInt("count_l", 5);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 model;
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", projection);
        shaderGeometryPass.setMat4("view", view);
        model = glm::mat4();
        model = glm::translate(model, landscapePos);
        model = glm::scale(model, glm::vec3(0.03f));
        shaderGeometryPass.setMat4("model", model);
        landscape.Draw(shaderGeometryPass);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderLightingPass.use();
        shaderLightingPass.setInt("mode", mode);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        shaderLightingPass.setInt("count_l", lights.getCount());
        for (unsigned int i = 0; i < lightPositions.size(); i++) {
            lightPositions[i].x += cos((float) glfwGetTime()) * 0.02f;
            lightPositions[i].z += sin((float) glfwGetTime()) * 0.02f;
        }
        lights.refresh(lightPositions, lightColors, shaderLightingPass);
        shaderLightingPass.setVec3("viewPos", positionCamera);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (mode == 0) {
            lights.draw(lightPositions, lightColors, sphere, shaderLightBox, projection, view);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}