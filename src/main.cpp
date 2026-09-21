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

void updatePerformanceCounter(GLFWwindow* window);
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

glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 1.0f);
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

    Shader modelShader("resources/shaders/3.3.model_loading.vs", "resources/shaders/3.3.model_loading.fs");
    Shader lightCubeShader("resources/shaders/3.3.light_cube.vs", "resources/shaders/3.3.light_cube.fs");

    Model ourModel("resources/objects/2b-in-kimono/28.glb");
    //Model ourModel("resources/objects/ijichi-nijika/1.fbx");

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(lightCubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


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

        // orbit
        float loopWidth = 1.0;
        lightPos.x = loopWidth * cos(glfwGetTime());
        lightPos.y = (loopWidth / 2) * sin(2 * glfwGetTime());
        lightPos.z = sin(glfwGetTime());

        // see polygons
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // render 3d model
        modelShader.use();
        modelShader.setVec3("light.position", lightPos);
        modelShader.setVec3("viewPos", camera.Position);

        // light properties
        modelShader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f); // note that all light colors are set at full intensity
        modelShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
        modelShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // material properties
        modelShader.setVec3("material.ambient", 0.1f, 0.1f, 0.1f);
        modelShader.setVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
        modelShader.setVec3("material.specular", 0.50196078f, 0.50196078f, 0.50196078f);
        modelShader.setFloat("material.shininess", 16.0f);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(1.5f));
        modelShader.setMat4("model", model);
        ourModel.Draw(modelShader);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setVec3("lightColor", lightColor);

        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.15f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();

        updatePerformanceCounter(window);
    }

    glfwTerminate();
    return 0;
}

void updatePerformanceCounter(GLFWwindow* window) {
    static float lastTime = static_cast<float>(glfwGetTime());
    static int frameCount = 0;

    float currentTime = glfwGetTime();
    frameCount++;

    if (currentTime - lastTime >= 1.0) {
        float fps = float(frameCount) / (currentTime - lastTime);
        float msPerFrame = 1000.0f / float(frameCount);

        std::string title = "main | fps: " + std::to_string(int(fps)) +
            " | frame time: " + std::to_string(msPerFrame) + " /ms";

        glfwSetWindowTitle(window, title.c_str());

        frameCount = 0;
        lastTime = currentTime;
    }
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void toggleFullscreen(GLFWwindow* window)
{
    if (!isFullscreen)
    {
        glfwGetWindowPos(window, &windowedX, &windowedY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else
        glfwSetWindowMonitor(window, NULL, windowedX, windowedY, windowedWidth, windowedHeight, 0);
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