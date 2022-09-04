#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec4 vertColor;
out vec2 texCoord;

uniform float time;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);

    vec3 col = 0.5f + 0.5f * cos(time + aPos.xyx + vec3(0, 2, 4));
    vertColor = vec4(col, 1.0f);

    texCoord = aTexCoord;
}
