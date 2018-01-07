#pragma once
#include "shader.h"
#include <vector>


extern float getRnd();
class Light
{
private:
    int _count;
    const int MAX = 128;
public:
    explicit Light(int count)
        : _count(count)
    {

    }
    void refresh(std::vector<glm::vec3>& lightPositions, std::vector<glm::vec3>& lightColors, Shader& shader)
    {
        for (unsigned int i = 0; i < lightPositions.size(); i++) {
            shader.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
            shader.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
            const float constant = 1.0;
            const float linear = 0.7;
            const float quadratic = 1.8;
            shader.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
            shader.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
            const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
            float radius = (-linear +
                            std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) /
                           (2.0f * quadratic);
            shader.setFloat("lights[" + std::to_string(i) + "].Radius", radius);
        }
    }

    void add(std::vector<glm::vec3>& lightPositions, std::vector<glm::vec3>& lightColors)
    {
        if (_count < lightPositions.size()) {
            _count++;
        } else {
            if (lightPositions.size() < MAX) {
                float xPos = ((rand() % 100) / 100.0) * 12.0 - 6.0;
                float yPos = -7.0;
                float zPos = ((rand() % 100) / 100.0) * 12.0 - 6.0;
                lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
                float rColor = getRnd();
                float gColor = getRnd();
                float bColor = (rColor - 1.0 <= 0.0 && gColor - 1.0 <= 0.0) ? 1.0 : 0.0;
                lightColors.push_back(glm::vec3(rColor, gColor, bColor));
            }
            _count++;
        }
    }

    void remove()
    {
        if (_count > 0)
            _count--;
    }

    int getCount()
    {
        return _count;
    }

    void draw(std::vector<glm::vec3>& lightPositions, std::vector<glm::vec3>& lightColors, Model& sphere, Shader& shader, glm::mat4& projection, glm::mat4& view)
    {
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        for (unsigned int i = 0; i < getCount(); i++) {
            glm::mat4 model = glm::mat4();
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(0.125f));
            shader.setMat4("model", model);
            shader.setVec3("lightColor", lightColors[i]);
            sphere.Draw(shader);
        }
    }
};