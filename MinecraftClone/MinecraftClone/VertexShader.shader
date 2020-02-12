#version 330 core

layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec2 textCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float rows;
uniform vec2 offset;

out vec2 vTextCoord;

void main()
{
	gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0); // note that we read the multiplication from right to left
	vTextCoord = (textCoord / rows) + offset;
}