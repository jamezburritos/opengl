#include <shader.h>

int shader_load(const char *source, GLenum type, shader *shd)
{
    shader temp = glCreateShader(type);
    glShaderSource(temp, 1, &source, NULL);
    glCompileShader(temp);

    int status;
    char infoLog[512];
    glGetShaderiv(temp, GL_COMPILE_STATUS, &status);

    if (!status) {
        glGetShaderInfoLog(temp, 512, NULL, infoLog);
        fprintf(stderr, "Failed to compile shader:\n%s\n", infoLog);
        return 0;
    }

    *shd = temp;
    return 1;
}

int shader_loadFile(const char *path, GLenum type, shader *shd)
{
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Failed to open file.");
        return 0;
    }

    char *buf;
    size_t len;

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buf = (char *) malloc(len + 1);
    fread(buf, 1, len, fp);
    buf[len] = '\0';
    fclose(fp);

    int status = shader_load(buf, type, shd);
    free(buf);

    return status;
}

int shader_createProgram(program *prog, int len, ...)
{
    va_list shaders;
    va_start(shaders, len);

    program temp = glCreateProgram();
    for (int i = 0; i < len; i++) {
        glAttachShader(temp, va_arg(shaders, shader));
    }
    glLinkProgram(temp);

    int status;
    char infoLog[512];
    glGetProgramiv(temp, GL_LINK_STATUS, &status);

    if (!status) {
        glGetProgramInfoLog(temp, 512, NULL, infoLog);
        printf("Failed to link program:\n%s\n", infoLog);
    }

    *prog = temp;
    return 1;
}
