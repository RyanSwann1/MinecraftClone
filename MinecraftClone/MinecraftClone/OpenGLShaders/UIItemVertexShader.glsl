#version 330 core

layout(location = 0) in vec2 aPos; 
layout(location = 1) in vec3 textCoord;

uniform mat4 uProjection;

out vec3 vTextCoord;

void main()
{
	gl_Position = uProjection * vec4(aPos.xy, 0.0, 1.0);
	vTextCoord = textCoord;
}