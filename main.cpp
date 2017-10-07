#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <glm/glm.hpp>
#include <AntTweakBar.h>

const uint32_t img[] = {
        0, 0, 262144, 786432, 1048576, 1572864, 2097152, 2359296, 2883584, 3145728, 3670016, 4194304, 4456448, 4980736,
        5242880, 5767168, 6291456, 6553600, 7077888, 7602176, 7864320, 8388608, 8650752, 9175040, 9699328, 9961472,
        10485760, 10747904, 11272192, 11796480, 12058624, 12582912, 13107200, 13108224, 13110272, 13373440, 13375488,
        13638656, 13640704, 13641728, 13905920, 13907968, 14171136, 14173184, 14174208, 14438400, 14439424, 14703616,
        14705664, 14706688, 14970880, 14971904, 15236096, 15237120, 15239168, 15502336, 15504384, 15768576, 15769600,
        15771648, 16034816, 16036864, 16300032, 16302080, 16566272, 16566276, 16566284, 16567316, 16567324, 16568356,
        16568364, 16568372, 16569404, 16569412, 16570444, 16570452, 16570460, 16571492, 16571500, 16572532, 16572540,
        16572548, 16573580, 16573588, 16574620, 16574628, 16574636, 16575668, 16575676, 16576708, 16576716, 16576724,
        16577756, 16577764, 16578796, 16578804, 16579836, 16579832, 16579828, 16579824, 16579816, 16579812, 16579808,
        16579800, 16579796, 16579792, 16579784, 16579780, 16579776, 16579768, 16579764, 16579760, 16579752, 16579748,
        16579744, 16579740, 16579732, 16579728, 16579724, 16579716, 16579712, 16579708, 16579700, 16579696, 16579692,
        16579684, 16579680, 16579676, 16579668, 16579664, 16579660, 16579656, 16579648, 16579644, 16579640, 16579632,
        16579628, 16579624, 16579616, 16579612, 16579608, 16579600, 16579596, 16579592, 16579584, 16578560, 16577536,
        16576512, 16574464, 16573440, 16572416, 16570368, 16569344, 16568320, 16566272, 16565248, 16564224, 16562176,
        16561152, 16560128, 16558080, 16557056, 16556032, 16555008, 16552960, 16551936, 16550912, 16548864, 16547840,
        16546816, 16544768, 16543744, 16542720, 16540672, 16539648, 16538624, 16536576, 16535552, 16534528, 16533504,
        16531456, 16530432, 16529408, 16527360, 16526336, 16525312, 16523264, 16522240, 16521216, 16519168, 16518144,
        16517120, 16515072, 16252928, 15990784, 15728640, 15466496, 15204352, 14942208, 14680064, 14417920, 14155776,
        13893632, 13631488, 13369344, 13107200, 12845056, 12582912, 12320768, 12058624, 11796480, 11534336, 11272192,
        11010048, 10747904, 10485760, 10223616, 9961472, 9699328, 9437184, 9175040, 8912896, 8650752, 8388608, 8126464,
        7864320, 7602176, 7340032, 7077888, 6815744, 6553600, 6291456, 6029312, 5767168, 5505024, 5242880, 4980736,
        4718592, 4456448, 4194304, 3932160, 3670016, 3407872, 3145728, 2883584, 2621440, 2359296, 2097152, 1835008,
        1572864, 1310720, 1048576, 786432, 524288, 0, 0
};

TwBar *bar;
const GLuint WIDTH = 800, HEIGHT = 600;
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


inline void TwEventMouseButtonGLFW3(GLFWwindow* window, int button, int action, int mods)
{
    TwEventMouseButtonGLFW(button, action);
    updateStart = isPress = button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS;
    isPressBar = false;
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
            const double dx = (xpos - X_old) / (double)WIDTH * zoom * WIDTH / HEIGHT;
            const double dy = (ypos - Y_old) / (double)HEIGHT * zoom;
            X += dx;
            Y += dy;
            X_old = xpos;
            Y_old = ypos;
        }
    }
}
inline void TwEventMouseWheelGLFW3(GLFWwindow* window, double xoffset, double yoffset)
{
    TwEventMouseWheelGLFW(yoffset);
    oldZoom = zoom;
    zoom += yoffset * 0.001 * zoom;
    zoom = std::max(0.001f, zoom);
    const double oldX = -(zoom_X / (double)WIDTH - 0.5) * oldZoom;
    const double oldY = (zoom_Y / (double)HEIGHT - 0.5) * oldZoom;

    const double newX = -(zoom_X / (double)WIDTH - 0.5) * zoom;
    const double newY = (zoom_Y / (double)HEIGHT - 0.5) * zoom;
    X -= (newX - oldX) * WIDTH / HEIGHT;
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

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Fractal", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    TwInit(TW_OPENGL_CORE, nullptr);
    bar = TwNewBar("TweakBar");
    TwWindowSize(1600, 1200);
    TwAddVarRW(bar, "Iteration", TW_TYPE_INT32, &iter,
               " label='Iteration' min=1 max=500");
    glfwSetWindowSizeCallback(window, (GLFWwindowposfun)TwWindowSizeGLFW3);

    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
    glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW3);
    glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW3);
    glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW3);
    glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW3);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    Shader ourShader("../vertex.glsl",
                     "../fragment.glsl");
    GLfloat vertices[] = {
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    GLuint indices[] = {
            0, 1, 3,
            1, 2, 3
    };
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, img);
    glBindTexture(GL_TEXTURE_1D, 0);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ourShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, texture);
        glUniform1i(glGetUniformLocation(ourShader.Program, "tex"), 0);

        GLint s = glGetUniformLocation(ourShader.Program, "scale");
        GLint i = glGetUniformLocation(ourShader.Program, "iter");
        GLint c = glGetUniformLocation(ourShader.Program, "center");

        glUniform1i(i, iter);
        glUniform1f(s, zoom);
        glUniform2f(c, X, Y);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        TwDraw();
        glfwSwapBuffers(window);
    }
    TwTerminate();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}