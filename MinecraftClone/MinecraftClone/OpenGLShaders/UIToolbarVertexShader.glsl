#version 330 core

layout(location = 0) in vec2 aPos; 
layout(location = 1) in vec2 textCoord;

uniform mat4 uModel;
uniform mat4 uProjection;

out vec2 vTextCoord;

void main()
{
	gl_Position = uProjection * uModel * vec4(aPos.xy, 0.0, 1.0);
	vTextCoord = textCoord;
}