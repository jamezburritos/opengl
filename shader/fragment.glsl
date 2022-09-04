#version 330 core

in vec4 vertColor;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D ourTexture;

void main()
{
    fragColor = texture(ourTexture, texCoord);
}
