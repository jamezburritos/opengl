#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>
#include <cglm/call.h>

#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// vertex data
float vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left
};

// indexed vertices
unsigned int indices[] = {
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

char* loadFile(const char *filename);
void framebuffer_resize_callback(GLFWwindow *window, int w, int h);

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
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create window.\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

    // glad: load GL functions
    if (!gladLoadGL())
    {
        printf("Failed to initialise GLAD.");
        return -1;
    }

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
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
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
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
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
        printf("ERROR::SHADER::PROGRAM::LINK_FAILED\n%s\n", infoLog);
    }

    // clean up shaders
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    /*
     * Process the event-loop
     */

    // set the default background color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        // render the frame
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm_mat4_identity(model);
        glm_mat4_identity(view);
        glm_mat4_identity(projection);

        glm_rotate(model, glm_rad(-55), (vec3) { 1.0, 0.0, 0.0 });
        glm_rotate(model, glm_rad(-35), (vec3) { 0.0, 0.0, 1.0 });
        glm_translate(view, (vec3) { 0.0, 0.0, -2.0 });
        glm_perspective_default(1.0f, projection);

        glUniformMatrix4fv(u_model, 1, GL_FALSE, (float *) model);
        glUniformMatrix4fv(u_view, 1, GL_FALSE, (float *) view);
        glUniformMatrix4fv(u_projection, 1, GL_FALSE, (float *) projection);

        glUniform1f(u_time, glfwGetTime());

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
}
