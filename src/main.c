#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>
#include <cglm/io.h>

#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>

// convenience macros
#define X 0
#define Y 1
#define Z 2

// vertex data
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

// indexed vertices
unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
};

// window size
unsigned int window_w = 800;
unsigned int window_h = 600;

// camera
vec3 cameraPos, cameraDirection, cameraRight, cameraUp, cameraFront;
float pitch, roll, yaw, fov;

// time
float deltaTime;

// cursor
float curX, curY;

// forward-declarations
char* loadFile(const char *filename);

void framebuffer_resize_callback(GLFWwindow *window, int w, int h);
void cursor_callback(GLFWwindow *window, double x, double y);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

int main()
{
    /*
     * Initialise GLFW/GLAD
     */

    // glfw: start & set version
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw: create a window
    GLFWwindow *window = glfwCreateWindow(window_w, window_h, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create window.\n");
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load GL functions
    if (!gladLoadGL())
    {
        printf("Failed to initialise GLAD.");
        return -1;
    }

    /*
     * Load, generate and bind textures
     */

    int texWidth, texHeight, texChannels;
    unsigned char *data = stbi_load("texture.jpg", &texWidth, &texHeight, &texChannels, 0);

    if (!data) {
        printf("Failed to load texture from file.");
    }

    unsigned int texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    /*
     * Generate and compile shaders
     */

    int success;
    char infoLog[512];

    // vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    const char* vertexShaderSource = loadFile("./shader/vertex.glsl");

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex shader compilation failed:\n%s\n", infoLog);
    }

    // fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* fragmentShaderSource = loadFile("./shader/fragment.glsl");

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment shader compilation failed:\n%s\n", infoLog);
    }

    /*
     * Create a shader program and attach shaders.
     * Set up uniform values for the shaders.
     * Delete the shaders when it's done, we don't need them anymore
     */

    // shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glUseProgram(shaderProgram);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Program linking failed:\n%s\n", infoLog);
    }

    // clean up shaders
    free((void *) vertexShaderSource);
    free((void *) fragmentShaderSource);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /*
     * Set up uniform values.
     */

    unsigned int u_time = glGetUniformLocation(shaderProgram, "time");
    unsigned int u_model = glGetUniformLocation(shaderProgram, "model");
    unsigned int u_view = glGetUniformLocation(shaderProgram, "view");
    unsigned int u_projection = glGetUniformLocation(shaderProgram, "projection");

    mat4 model, view, projection;

    /*
     * Set up vertex data, configure vertex attrs.
     */

    // vertex array
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // vertex buffer
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // element-array buffer
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);                     // vertex coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));   // texture coords

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    /*
     * Setup camera
     */

    glm_vec3_zero(cameraPos);
    vec3 up = { 0.0, 1.0, 0.0 };

    fov = 45.0;

    /*
     * Process the event-loop
     */

    // set the default background color
    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        // process user input
        processInput(window);

        // work out frame time
        float lastFrame, currentTime;
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        // render the frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm_mat4_identity(model);
        glm_mat4_identity(view);
        glm_mat4_identity(projection);

        // model

        // view
        cameraDirection[X] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
        cameraDirection[Y] = sin(glm_rad(pitch));
        cameraDirection[Z] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));

        glm_vec3_normalize(cameraDirection);

        glm_cross(up, cameraDirection, cameraRight);
        glm_vec3_normalize(cameraRight);

        glm_cross(cameraDirection, cameraRight, cameraUp);

        printf("\033c");
        printf("pos: %.1f, %.1f, %.1f\n", cameraPos[X], cameraPos[Y], cameraPos[Z]);
        printf("pitch: %.1f, roll: %.1f, yaw: %.1f\n", pitch, roll, yaw);
        printf("fov: %.1f\n", fov);

        glm_look(cameraPos, cameraDirection, cameraUp, view);

        // projection
        glm_perspective(glm_rad(fov), (float) window_w / (float) window_h, 0.1, 100.0, projection);

        glUniformMatrix4fv(u_model, 1, GL_FALSE, (float *) model);
        glUniformMatrix4fv(u_view, 1, GL_FALSE, (float *) view);
        glUniformMatrix4fv(u_projection, 1, GL_FALSE, (float *) projection);

        glUniform1f(u_time, currentTime);

        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers, poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /*
     * Exit nicely
     */

    glfwTerminate();
    return 0;
}

char* loadFile(const char* filename)
{
    FILE *fp = fopen(filename, "r");

    char *buf;
    size_t flen;

    fseek(fp, 0, SEEK_END);
    flen = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buf = (char *) malloc(flen + 1);
    fread(buf, 1, flen, fp);

    buf[flen] = '\0';

    return buf;
}

void framebuffer_resize_callback(GLFWwindow *window, int w, int h)
{
    glViewport(0, 0, w, h);
    window_w = w;
    window_h = h;
}

void cursor_callback(GLFWwindow *window, double x, double y)
{
    float xoffset = x - curX;
    float yoffset = curY - y;
    curX = x;
    curY = y;

    float sensitivity = 0.1;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    pitch += yoffset;
    yaw += xoffset;

    if (pitch > 89.0)  pitch = 89.0;
    if (pitch < -89.0) pitch = -89.0;
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    fov -= (float) yoffset;

    if (fov < 1.0)  fov = 1.0;
    if (fov > 45.0) fov = 45.0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    const float cameraSpeed =
        ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 10.0 : 5.0) * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        vec3 transform;
        glm_vec3_scale(cameraDirection, cameraSpeed, transform);
        glm_vec3_add(cameraPos, transform, cameraPos);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        vec3 transform;
        glm_vec3_scale(cameraDirection, cameraSpeed, transform);
        glm_vec3_sub(cameraPos, transform, cameraPos);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 transform;
        glm_vec3_scale(cameraRight, cameraSpeed, transform);
        glm_vec3_add(cameraPos, transform, cameraPos);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 transform;
        glm_vec3_scale(cameraRight, cameraSpeed, transform);
        glm_vec3_sub(cameraPos, transform, cameraPos);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        vec3 transform;
        glm_vec3_scale(cameraUp, cameraSpeed, transform);
        glm_vec3_add(cameraPos, transform, cameraPos);
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        vec3 transform;
        glm_vec3_scale(cameraUp, cameraSpeed, transform);
        glm_vec3_sub(cameraPos, transform, cameraPos);
    }
}
