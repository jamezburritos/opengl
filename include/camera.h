#pragma once

/*
 * camera.h
 * define a camera type and functions to operate on it.
 */

#include <cglm/mat4.h>
#include <cglm/cam.h>

#define X 0
#define Y 1
#define Z 2

#define PITCH 0
#define ROLL 1
#define YAW 2

typedef struct {
    vec3 pos, up, right, dir, eulers;
    mat4 view;
} camera;

camera cam_create(vec3 pos, vec3 eulers);
void   cam_update(camera *cam);
void   cam_lookat(camera *cam, vec3 target);
