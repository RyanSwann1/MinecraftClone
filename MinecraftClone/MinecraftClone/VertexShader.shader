#version 330 core

layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec2 textCoord;

out vec2 vTextCoord;

void main()
{
	gl_Position = vec4(aPos, 1.0); 
	vTextCoord = textCoord;
}