#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/shader.h"
#include "include/model.h"
#include "include/light.h"
#include "include/camera.h"
#include <AntTweakBar.h>

bool typeMouse = false;
int df = 100;
TwBar *bar;
bool isPress = false;
bool isPressBar = false;
bool updateStart = false;
float oldZoom = 2.33;
float X = 0.7f, Y = 0.0f;
float X_old = 0.0f, Y_old = 0.0f;
float zoom_X = 0.0f;
float zoom_Y = 0.0f;
float zoom = 2.33;
int iter = 100;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;


float getRnd()
{
    return (float) (rand() % 100 > 50 ? 1.0 : 0.0);
}

inline void TwEventMouseButtonGLFW3(GLFWwindow* window, int button, int action, int mods)
{
    TwEventMouseButtonGLFW(button, action);
    updateStart = isPress = button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS;
    isPressBar = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    if (!typeMouse)
        camera.ProcessMouseMovement(xoffset, yoffset);
}


inline void TwEventMousePosGLFW3(GLFWwindow* window, double xpos, double ypos)
{
#ifdef __APPLE__
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    TwMouseMotion(int(xpos * width / WIDTH), int(ypos * height / HEIGHT));
#else
    TwMouseMotion(int(xpos), int(ypos));
#endif
    int pos[2];
    int size[2];
    TwGetParam(bar, nullptr, "position", TW_PARAM_INT32, 2, pos);
    TwGetParam(bar, nullptr, "size", TW_PARAM_INT32, 2, size);
#ifdef __APPLE__
    pos[0] /= width / WIDTH;
    pos[1] /= height / HEIGHT;
    size[0] /= width / WIDTH;
    size[1] /= height / HEIGHT;
#endif
    if ((xpos > pos[0] && xpos < size[0] + pos[0] && ypos > pos[1] && ypos < size[1] + pos[1]) && !isPressBar)
    {
        isPressBar = true;
    }
    zoom_X = xpos;
    zoom_Y = ypos;
    if (!(xpos > pos[0] && xpos < size[0] + pos[0] && ypos > pos[1] && ypos < size[1] + pos[1]) && !isPressBar)
    {
        if (updateStart)
        {
            X_old = xpos;
            Y_old = ypos;
            updateStart = false;
        }
        if (isPress)
        {
            const double dx = (xpos - X_old) / (double)SCR_WIDTH * zoom * SCR_WIDTH / SCR_HEIGHT;
            const double dy = (ypos - Y_old) / (double)SCR_HEIGHT * zoom;
            X += dx;
            Y += dy;
            X_old = xpos;
            Y_old = ypos;
        }
    }
    mouse_callback(window, xpos, ypos);
}
inline void TwEventMouseWheelGLFW3(GLFWwindow* window, double xoffset, double yoffset)
{
    TwEventMouseWheelGLFW(yoffset);
    oldZoom = zoom;
    zoom += yoffset * 0.001 * zoom;
    zoom = std::max(0.001f, zoom);
    const double oldX = -(zoom_X / (double)SCR_WIDTH - 0.5) * oldZoom;
    const double oldY = (zoom_Y / (double)SCR_HEIGHT - 0.5) * oldZoom;

    const double newX = -(zoom_X / (double)SCR_WIDTH - 0.5) * zoom;
    const double newY = (zoom_Y / (double)SCR_HEIGHT - 0.5) * zoom;
    X -= (newX - oldX) * SCR_WIDTH / SCR_HEIGHT;
    Y += newY - oldY;
}
inline void TwEventKeyGLFW3(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    TwEventKeyGLFW(key, action);
}
inline void TwEventCharGLFW3(GLFWwindow* window, int codepoint)
{
    TwEventCharGLFW(codepoint, GLFW_PRESS);
}
inline void TwWindowSizeGLFW3(GLFWwindow* window, int width, int height)
{
    TwWindowSize(width, height);
}


std::vector<glm::vec3> lightPositions;
std::vector<glm::vec3> lightColors;
std::vector<int> lightDif;
Light lights(1);
float g = 2.2f;
int mode = 0;
float deltaTime = 0.0f;

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        lights.remove();
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        lights.add(lightPositions, lightColors, lightDif);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        mode = 0;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        mode = 1;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        mode = 2;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        mode = 3;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        typeMouse = true;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        typeMouse = false;
    }
}

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

int main() {
    initialization();
    GLFWwindow *window = createWindow();
    TwInit(TW_OPENGL_CORE, nullptr);
    bar = TwNewBar("TweakBar");
    TwWindowSize(1280, 720);
    TwAddVarRW(bar, "Gamma", TW_TYPE_FLOAT, &g,
               " label='Gamma' min=0.1 max=10.0 step=0.1");

    glfwSetWindowSizeCallback(window, (GLFWwindowposfun)TwWindowSizeGLFW3);

    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
    glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW3);
    glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW3);
    glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW3);
    glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW3);
    initRenderScene();

    Shader shaderGeometryPass("shaders/g_buffer_vs.glsl", "shaders/g_buffer_fs.glsl");
    Shader shaderLightingPass("shaders/deferred_shading_vs.glsl", "shaders/deferred_shading_fs.glsl");
    Shader shaderLightBox("shaders/light_sphere_vs.glsl", "shaders/light_sphere_fs.glsl");
    Model sphere("sphere/sphere.obj");
    Model landscape("landscape/landscape.obj");
    initGBuffer();
    const unsigned int NR_LIGHTS = 1;
    srand(time(nullptr));
    for (unsigned int i = 0; i < NR_LIGHTS; i++) {
        float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        float yPos = -6.0;
        float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
        float rColor = getRnd();
        float gColor = getRnd();
        float bColor = (rColor - 1.0 <= 0.0 && gColor - 1.0 <= 0.0) ? 1.0 : 0.0;
//        float gColor = ((rand() % 100) / 200.0f) + 0.5;
//        float bColor = ((rand() % 100) / 200.0f) + 0.5;
        lightColors.push_back(glm::vec3(rColor, gColor, bColor));
        lightDif.push_back(df);
    }
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedoSpec", 2);
    shaderLightingPass.setInt("mode", mode);
    shaderLightingPass.setInt("count_l", 1);
    shaderLightingPass.setFloat("gamma", g);
    while (!glfwWindowShouldClose(window)) {
        if (typeMouse) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        float currentFrame = (float) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 model;
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
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
        shaderLightingPass.setFloat("gamma", g);
        df = rand() % 200;
        if (df == 0) {
            df = 100;
        }
        shaderLightingPass.setInt("dif_coef", df);
        for (unsigned int i = 0; i < lightPositions.size(); i++) {
            lightPositions[i].x += cos((float) glfwGetTime()) * 0.02f;
            lightPositions[i].z += sin((float) glfwGetTime()) * 0.02f;
        }
        lights.refresh(lightPositions, lightColors, lightDif, shaderLightingPass);
        shaderLightingPass.setVec3("viewPos", camera.Position);
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
        TwDraw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}