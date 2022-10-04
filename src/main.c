#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>
#include <cglm/io.h>

#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "camera.h"
#include "shader.h"

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

// window size
unsigned int window_w = 800;
unsigned int window_h = 600;

// camera
camera cam;
float fov;

// time
float deltaTime;

// cursor
float curX, curY;

// forward-declarations
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialise GLAD.");
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

    shader vertexShader, fragmentShader;
    shader_loadFile("./shader/vertex.glsl", GL_VERTEX_SHADER, &vertexShader);
    shader_loadFile("./shader/fragment.glsl", GL_FRAGMENT_SHADER, &fragmentShader);

    /*
     * Create a shader program and attach shaders.
     * Delete the shaders when it's done, we don't need them anymore
     */

    program shaderProgram;
    shader_createProgram(&shaderProgram, 2, vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /*
     * Set up uniform values.
     */

    uniform u_time = glGetUniformLocation(shaderProgram, "time");
    uniform u_model = glGetUniformLocation(shaderProgram, "model");
    uniform u_view = glGetUniformLocation(shaderProgram, "view");
    uniform u_projection = glGetUniformLocation(shaderProgram, "projection");

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


    // vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);                     // vertex coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));   // texture coords

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    /*
     * Setup camera
     */

    cam = cam_create((vec3) { 0, 0, 0 }, (vec3) { 0, 0, 0 });
    fov = 45;

    /*
     * Process the event-loop
     */

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
        glm_mat4_identity(projection);

        // model

        // view
        cam_update(&cam);

        printf("\033c");
        printf("pos: %.1f, %.1f, %.1f\n", cam.pos[X], cam.pos[Y], cam.pos[Z]);
        printf("pitch: %.1f, roll: %.1f, yaw: %.1f\n", cam.eulers[PITCH], cam.eulers[ROLL], cam.eulers[YAW]);
        printf("fov: %.1f\n", fov);

        // projection
        glm_perspective(glm_rad(fov), (float) window_w / (float) window_h, 0.1, 100.0, projection);

        glUniformMatrix4fv(u_model, 1, GL_FALSE, (float *) model);
        glUniformMatrix4fv(u_view, 1, GL_FALSE, (float *) cam.view);
        glUniformMatrix4fv(u_projection, 1, GL_FALSE, (float *) projection);

        glUniform1f(u_time, currentTime);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers, poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /*
     * Exit cleanly
     */

    glfwTerminate();
    return 0;
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

    cam.eulers[PITCH] += yoffset;
    cam.eulers[YAW] += xoffset;

    if (cam.eulers[PITCH] > 89.0)  cam.eulers[PITCH] = 89.0;
    if (cam.eulers[PITCH] < -89.0) cam.eulers[PITCH] = -89.0;

    cam.eulers[YAW] = fmod(cam.eulers[YAW], 360.0f);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    fov -= (float) yoffset;

    if (fov < 1.0)  fov = 1.0;
    if (fov > 45.0) fov = 45.0;
}

void processInput(GLFWwindow *window)
{
    #define key(KEYNAME) if (glfwGetKey(window, KEYNAME) == GLFW_PRESS)

    key(GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, 1);
    }

    const float cameraSpeed =
        ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 10.0 : 5.0) * deltaTime;

    key(GLFW_KEY_W) {
        vec3 transform;
        glm_vec3_scale(cam.dir, cameraSpeed, transform);
        glm_vec3_add(cam.pos, transform, cam.pos);
    }

    key(GLFW_KEY_S) {
        vec3 transform;
        glm_vec3_scale(cam.dir, cameraSpeed, transform);
        glm_vec3_sub(cam.pos, transform, cam.pos);
    }

    key(GLFW_KEY_A) {
        vec3 transform;
        glm_vec3_scale(cam.right, cameraSpeed, transform);
        glm_vec3_add(cam.pos, transform, cam.pos);
    }

    key(GLFW_KEY_D) {
        vec3 transform;
        glm_vec3_scale(cam.right, cameraSpeed, transform);
        glm_vec3_sub(cam.pos, transform, cam.pos);
    }

    key(GLFW_KEY_SPACE) {
        vec3 transform;
        glm_vec3_scale(cam.up, cameraSpeed, transform);
        glm_vec3_add(cam.pos, transform, cam.pos);
    }

    key(GLFW_KEY_C) {
        vec3 transform;
        glm_vec3_scale(cam.up, cameraSpeed, transform);
        glm_vec3_sub(cam.pos, transform, cam.pos);
    }

    key(GLFW_KEY_Z) {
        cam_lookat(&cam, (vec3) { 0, 0, 0 });
    }
}
