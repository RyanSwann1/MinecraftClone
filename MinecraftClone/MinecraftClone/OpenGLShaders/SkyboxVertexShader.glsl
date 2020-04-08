#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    TexCoords = aPos;
    vec4 position = uProjection * uView * vec4(aPos, 1.0);
    gl_Position = position.xyww;
}  