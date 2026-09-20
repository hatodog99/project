#include "../includes/glad/glad.h"
#include "../includes/GLFW/glfw3.h"

#include "../includes/glm/glm.hpp"
#include "../includes/glm/gtc/matrix_transform.hpp"
#include "../includes/glm/gtc/type_ptr.hpp"

#include "../headers/shader_s.h"
#include "../headers/model.h"
#include "../headers/camera.h"

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
    DOWN_ARROW,
    F11
};

bool wasPressed[10] = { false };

void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void toggleFullscreen(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void printInput(Input input);
unsigned int loadTexture(
    const char* path, 
    GLenum wrapMode = GL_REPEAT,
    GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR,
    GLenum magFilter = GL_LINEAR_MIPMAP_LINEAR  
);

// screen size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

bool isFullscreen = false;
int windowedX, windowedY, windowedWidth, windowedHeight;

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
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "main", NULL, NULL);
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

    Shader ourShader("resources/shaders/3.3.model_loading.vs", "resources/shaders/3.3.model_loading.fs");
    //Model ourModel("resources/objects/2b-in-kimono/28.glb");
    Model ourModel("resources/objects/ijichi-nijika/1.fbx");

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        processInput(window);

        // background
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render cube
        ourShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.01f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void toggleFullscreen(GLFWwindow* window)
{
    if (!isFullscreen)
    {
        // remember current windowed position/size before switching
        glfwGetWindowPos(window, &windowedX, &windowedY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        int monitorX, monitorY;
        glfwGetMonitorPos(monitor, &monitorX, &monitorY);
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
        glfwSetWindowMonitor(window, NULL, monitorX, monitorY, mode->width, mode->height, 0);
    }
    else
    {
        glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
        glfwSetWindowMonitor(window, NULL, windowedX, windowedY, windowedWidth, windowedHeight, 0);
    }
    isFullscreen = !isFullscreen;
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
    bool escDown = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    if (escDown)
    {
        if (!wasPressed[ESC]) printInput(ESC);
        glfwSetWindowShouldClose(window, true);
    }
    wasPressed[ESC] = escDown;

    // movement
    bool wDown = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    if (wDown)
    {
        if (!wasPressed[W]) printInput(W);
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    wasPressed[W] = wDown;

    bool sDown = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    if (sDown)
    {
        if (!wasPressed[S]) printInput(S);
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    wasPressed[S] = sDown;

    bool aDown = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    if (aDown)
    {
        if (!wasPressed[A]) printInput(A);
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    wasPressed[A] = aDown;

    bool dDown = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
    if (dDown)
    {
        if (!wasPressed[D]) printInput(D);
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    wasPressed[D] = dDown;

    bool spaceDown = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (spaceDown)
    {
        if (!wasPressed[SPACE]) printInput(SPACE);
        camera.ProcessKeyboard(UP, deltaTime);
    }
    wasPressed[SPACE] = spaceDown;

    bool shiftDown = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    if (shiftDown)
    {
        if (!wasPressed[LEFT_SHIFT]) printInput(LEFT_SHIFT);
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    wasPressed[LEFT_SHIFT] = shiftDown;

    // texture mix
    mixCooldownTimer += deltaTime;
    bool upDown = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
    if (upDown && mixCooldownTimer >= inputCooldownTime)
    {
        if (!wasPressed[UP_ARROW]) printInput(UP_ARROW);
        mixValue += 0.05f;
        if (mixValue >= 1.0f) mixValue = 1.0f;
        mixCooldownTimer = 0.0f;
    }
    wasPressed[UP_ARROW] = upDown;

    bool downDown = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
    if (downDown && mixCooldownTimer >= inputCooldownTime)
    {
        if (!wasPressed[DOWN_ARROW]) printInput(DOWN_ARROW);
        mixValue -= 0.05f;
        if (mixValue <= 0.0f) mixValue = 0.0f;
        mixCooldownTimer = 0.0f;
    }
    wasPressed[DOWN_ARROW] = downDown;

    bool f11Down = glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS;
    if (f11Down && !wasPressed[F11]) {// add F11 to your Input enum + bump wasPressed array size
        printInput(F11);
        toggleFullscreen(window);
    }
    wasPressed[F11] = f11Down;
}

void printInput(Input input)
{
    std::string currentInput = "";

    if (input == ESC) 
        currentInput = "ESC";
    if (input == W) 
        currentInput = "W";
    if (input == S) 
        currentInput = "S";
    if (input == A) 
        currentInput = "A";
    if (input == D) 
        currentInput = "D";
    if (input == SPACE) 
        currentInput = "SPACE";
    if (input == LEFT_SHIFT) 
        currentInput = "LEFT_SHIFT";
    if (input == UP_ARROW) 
        currentInput = "ARROW_UP";
    if (input == DOWN_ARROW) 
        currentInput = "ARROW_DOWN";
    if (input == F11) 
        currentInput = "F11";

    std::cout << "\rinput: " << currentInput << "          " << std::flush;
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
        GLenum format;
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