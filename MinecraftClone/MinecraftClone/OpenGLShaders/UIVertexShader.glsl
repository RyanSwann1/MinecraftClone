#version 330 core

layout(location = 0) in vec2 aPos; 
layout(location = 1) in vec3 textCoord;

out vec3 vTextCoord;

void main()
{
	gl_Position = vec4(aPos.xy, 1.0, 1.0);
	vTextCoord = textCoord;
}