#define __CL_ENABLE_EXCEPTIONS
#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <QImage>
#include <QGLWidget>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "include/cl.hpp"
#include "include/shader.h"

const int WIDTH = 600;
const int HEIGHT = 600;
int x_coef = 1;
int y_coef = 1;
int current_x = 0;
int current_y = 0;
const float SCALE_FACTOR = 0.90f;
glm::fvec2 centerStart;
glm::fvec2 center(0.0f);
float scale = 1.0f;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 mvp = glm::mat4(1.0f);
bool mouse_button_press = false;
int mode = 0;


void changeMatrix() {
    model = glm::scale(glm::mat4(1.0f), glm::fvec3(scale, scale, 1.0f))
            * glm::translate(glm::mat4(1.0f), glm::fvec3(-center.x, -center.y, 0.0f));
    mvp = model;
}

glm::fvec2 unproject(const glm::fvec2 &win) {
    glm::ivec4 viewPort;
    glGetIntegerv(GL_VIEWPORT, &viewPort[0]);
    return glm::fvec2(glm::unProject(glm::fvec3(win, 0.0f), model, glm::mat4(1.0f), viewPort));
}

void mouseMove(int x, int y) {
    y = (HEIGHT - y);
    changeMatrix();
    glm::fvec2 cur = unproject(glm::fvec2(x, y));
    center += (centerStart - cur);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    xpos *= x_coef;
    ypos *= y_coef;
    current_x = xpos;
    current_y = ypos;
    if (mouse_button_press) {
        mouseMove(xpos, ypos);
    } else {
        centerStart = unproject(glm::fvec2(xpos, HEIGHT - ypos));
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouse_button_press = true;
    } else {
        mouse_button_press = false;
    }
}

void mouseWheel(int direction, int x, int y) {
    changeMatrix();
    glm::fvec2 pos = glm::fvec2(x, y);
    glm::fvec2 beforeZoom = unproject(pos);
    if (direction == -1) {
        scale *= SCALE_FACTOR;
        if (scale < 1.0) {
            scale = 1.0;
        }
    } else {
        scale /= SCALE_FACTOR;
    }

    changeMatrix();
    glm::fvec2 afterZoom = unproject(pos);
    center += beforeZoom - afterZoom;
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (yoffset < 0)
        mouseWheel(-1, current_x, (HEIGHT - current_y));
    if (yoffset > 0) {
        mouseWheel(1, current_x, (HEIGHT - current_y));
    }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int _mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        mode = 0;
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        mode = 1;
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        mode = 2;
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
        mode = 3;

}

void initialization() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_STENCIL_BITS, 4);
    glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

GLFWwindow *createWindow() {
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "3D", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return nullptr;
    }
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    x_coef = width / WIDTH;
    y_coef = height / HEIGHT;
    return window;
}

const double INF = 99999.0;
const double EPS = 1e-6;

int clp2(int n, int block_size)
{
    int block = block_size;
    while (block < n)
    {
        block += block_size;
    }
    return block;
}

QImage test(QImage&& img, int width, int height) {
    int w = img.width(), h = img.height();
    std::cout << w << " " << h;
    float** df_1 = new float*[h];
    df_1[0] = new float[h * w];

    float** df_1_out = new float*[h];
    df_1_out[0] = new float[h * w];

    float** df_2 = new float*[h];
    df_2[0] = new float[h * w];

    float** df_2_out = new float*[h];
    df_2_out[0] = new float[h * w];

    for (int i = 1; i < h; ++i)
    {
        df_1[i] = df_1[i - 1] + w;
        df_1_out[i] = df_1_out[i - 1] + w;
        df_2[i] = df_2[i - 1] + w;
        df_2_out[i] = df_2_out[i - 1] + w;
    }
    for (int x = 0; x < h; x++) {
        for (int y = 0; y < w; y++) {
            if (qGreen(img.pixel(y, x)) > 128) {
                df_1[x][y] = 0.0;
                df_2[x][y] = INF;
            } else {
                df_1[x][y] = INF;
                df_2[x][y] = 0.0;
            }
        }
    }

    std::vector<cl::Platform> platforms;
    std::vector<cl::Device> devices;
    std::vector<cl::Kernel> kernels;

    try {
        cl::Platform::get(&platforms);
        platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);

        cl::Context context(devices);

        std::vector<cl::Device> d = context.getInfo<CL_CONTEXT_DEVICES>();

        cl::CommandQueue queue(context, devices[0]);

        std::ifstream cl_file("../convolution.cl");
        std::string cl_string(std::istreambuf_iterator<char>(cl_file), (std::istreambuf_iterator<char>()));
        cl::Program::Sources source(1, std::make_pair(cl_string.c_str(), cl_string.length() + 1));

        cl::Program program(context, source);

        size_t const block_size = 16;
        program.build(devices, (std::string("-D BLOCK_SIZE=") + std::to_string(block_size)).c_str());

        cl::Buffer dev_first(context, CL_MEM_READ_ONLY,  sizeof(float) * h * w);
        cl::Buffer dev_first_out(context, CL_MEM_READ_ONLY,  sizeof(float) * h * w);
        queue.enqueueWriteBuffer(dev_first, CL_TRUE, 0, sizeof(float) * h * w, df_1[0]);

        cl::Kernel kernel(program, "df");
        int N = clp2(std::max(h, w), block_size);
        cl::KernelFunctor matrix_mult(kernel, queue, cl::NullRange, cl::NDRange(N, N), cl::NDRange(block_size, block_size));
        matrix_mult(dev_first, dev_first_out, w, h, 10);
        queue.enqueueReadBuffer(dev_first_out, CL_TRUE, 0, sizeof(float) * w * h, df_1_out[0]);

        queue.enqueueWriteBuffer(dev_first, CL_TRUE, 0, sizeof(float) * h * w, df_2[0]);
        matrix_mult(dev_first, dev_first_out, w, h, 10);
        queue.enqueueReadBuffer(dev_first_out, CL_TRUE, 0, sizeof(float) * w * h, df_2_out[0]);

    }catch (cl::Error& e)
    {
        std::cout << std::endl << e.what() << " : " << e.err() << std::endl;
    }

    QImage out(w, h, QImage::Format_ARGB32);
    for (int x = 0; x < h; x++)
        for (int y = 0; y < w; y++) {
            double dist1 = df_1_out[x][y];
            double dist2 = df_2_out[x][y];
            double dist = dist2 - dist1;
            int c = dist * 64 / 40 + 128;
            if (c < 0) c = 0;
            if (c > 255) c = 255;
            out.setPixel(y, x, qRgb(c, c, c));
        }
    delete df_1[0];
    delete []df_1;
    delete df_2[0];
    delete []df_2;
    delete df_1_out[0];
    delete []df_1_out;
    delete df_2_out[0];
    delete []df_2_out;
    return out.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        return -1;
    }
    auto img = test(QImage(argv[1]), 256, 256);
//    img.save("../test1.png", "png");

    initialization();
    GLFWwindow *window = createWindow();
    if (!window) {
        return -1;
    }
    Shader ourShader("../shaders/textures_vs.glsl", "../shaders/textures_frag.glsl");
    GLfloat vertices[] = {
            1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *) (6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    GLuint texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    QImage gl_img = QGLWidget::convertToGLFormat(img);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gl_img.width(), gl_img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 gl_img.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        ourShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        ourShader.setInt("ourTexture1", 0);
        ourShader.setInt("mode", mode);
        changeMatrix();
        ourShader.setMat4("mvpMatrix", mvp);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}