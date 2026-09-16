#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "shader_s.h"
#include "camera.h"

#include <iostream>

enum Input {
    ESC,
    W,
    S,
    A,
    D,
    SPACE,
    LEFT_SHIFT,
    UP_ARROW,
    DOWN_ARROW
};

void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void printInput(Input input);
unsigned int loadTexture
(
    const char* path, 
    GLenum wrapMode = GL_REPEAT,
    GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR,
    GLenum magFilter = GL_LINEAR_MIPMAP_LINEAR  
);

// screen size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouseInput = true;

// frame times
float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

// input times
float inputCooldownTime = 0.05f;
float cooldownTimer = 0.0f;
float mixCooldownTimer = 0.0f;

float mixValue = 0.3f;

glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "nijika", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader shader("shaders/3.3.shader.vs", "shaders/3.3.shader.fs");
    Shader lightCubeShader("shaders/3.3.light_cube.vs", "shaders/3.3.light_cube.fs");

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    //glm::vec3 cubePositions[] = {
    //    glm::vec3(0.0f,  0.0f,  0.0f),
    //    glm::vec3(2.0f,  5.0f, -15.0f),
    //    glm::vec3(-1.5f, -2.2f, -2.5f),
    //    glm::vec3(-3.8f, -2.0f, -12.3f),
    //    glm::vec3(2.4f, -0.4f, -3.5f),
    //    glm::vec3(-1.7f,  3.0f, -7.5f),
    //    glm::vec3(1.3f, -2.0f, -2.5f),
    //    glm::vec3(1.5f,  2.0f, -2.5f),
    //    glm::vec3(1.5f,  0.2f, -1.5f),
    //    glm::vec3(-1.3f,  1.0f, -1.5f)
    //};

    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //stbi_set_flip_vertically_on_load(true);

    //unsigned int texture1 = loadTexture("textures/nijika.jpg");
    //unsigned int texture2 = loadTexture("textures/hearts1.jpg");

    //shader.use();
    //shader.setInt("texture1", 0);
    //shader.setInt("texture2", 1);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        processInput(window);

        // background
        glClearColor(0.085f, 0.085f, 0.085f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind Texture
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, texture1);
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, texture2);

        // render cube
        shader.use();
        shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn)
{
    float xpos = static_cast<float>(xPosIn);
    float ypos = static_cast<float>(yPosIn);

    if (firstMouseInput)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouseInput = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        printInput(ESC);
        glfwSetWindowShouldClose(window, true);
    }

    // movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        printInput(W);
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        printInput(S);
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        printInput(A);
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        printInput(D);
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        printInput(SPACE);
        camera.ProcessKeyboard(UP, deltaTime);

    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        printInput(LEFT_SHIFT);
        camera.ProcessKeyboard(DOWN, deltaTime);
    }

    // texture mix
    mixCooldownTimer += deltaTime;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && mixCooldownTimer >= inputCooldownTime)
    {
        printInput(UP_ARROW);
        mixValue += 0.05f;
        if (mixValue >= 1.0f) mixValue = 1.0f;
        mixCooldownTimer = 0.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && mixCooldownTimer >= inputCooldownTime)
    {
        printInput(DOWN_ARROW);
        mixValue -= 0.05f;
        if (mixValue <= 0.0f) mixValue = 0.0f;
        mixCooldownTimer = 0.0f;
    }
}

void printInput(Input input)
{
    std::string currentInput = "";

    if (input == ESC) currentInput = "ESC";
    if (input == W) currentInput = "W";
    if (input == S) currentInput = "S";
    if (input == A) currentInput = "A";
    if (input == D) currentInput = "D";
    if (input == SPACE) currentInput = "SPACE";
    if (input == LEFT_SHIFT) currentInput = "LEFT_SHIFT";
    if (input == UP_ARROW) currentInput = "ARROW_UP";
    if (input == DOWN_ARROW) currentInput = "ARROW_DOWN";

    std::cout << "input: " << currentInput << std::endl;
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(const char* path, GLenum wrapMode, GLenum minFilter, GLenum magFilter)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}