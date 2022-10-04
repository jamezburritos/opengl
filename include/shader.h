#pragma once

/*
 * shader.h
 * define a shader type and functions to load shaders.
 * define a program type and functions to operate on it.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <glad/glad.h>

typedef unsigned int shader;
typedef unsigned int program;
typedef unsigned int uniform;

int shader_load(const char *source, GLenum type, shader *shd);
int shader_loadFile(const char *path, GLenum type, shader *shd);
int shader_createProgram(program *prog, int len, ...);
