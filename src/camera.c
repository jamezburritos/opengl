#include "camera.h"

camera cam_create(vec3 pos, vec3 eulers)
{
    camera cam;
    glm_vec3_copy(pos, cam.pos);
    glm_vec3_copy(eulers, cam.eulers);

    cam_update(&cam);

    return cam;
}

void cam_update(camera *cam)
{
    // calculate direction from eulers
    cam->dir[X] = cos(glm_rad(cam->eulers[YAW])) * cos(glm_rad(cam->eulers[PITCH]));
    cam->dir[Y] = sin(glm_rad(cam->eulers[PITCH]));
    cam->dir[Z] = sin(glm_rad(cam->eulers[YAW])) * cos(glm_rad(cam->eulers[PITCH]));

    glm_vec3_normalize(cam->dir);

    // calculate right and up vectors
    vec3 up = { 0, 1, 0 };

    glm_cross(up, cam->dir, cam->right);
    glm_vec3_normalize(cam->right);

    glm_cross(cam->dir, cam->right, cam->up);

    // calculate view matrix
    glm_mat4_identity(cam->view);
    glm_look(cam->pos, cam->dir, cam->up, cam->view);
}

void cam_lookat(camera *cam, vec3 target)
{
    // calculate view matrix
    glm_mat4_identity(cam->view);
    glm_lookat(cam->pos, target, (vec3) { 0, 1, 0 } /*cam->up*/,cam->view);

    // calculate eulers from vec3 matrix
    // umm idk which angle is which
    cam->eulers[PITCH] = glm_deg(atan2(cam->view[2][1], cam->view[2][2]));
    cam->eulers[ROLL] = glm_deg(atan2(-cam->view[2][0], sqrt(pow(cam->view[2][1], 2) + pow(cam->view[2][2], 2))));
    cam->eulers[YAW] = glm_deg(atan2(cam->view[1][0], cam->view[0][0]));
}
