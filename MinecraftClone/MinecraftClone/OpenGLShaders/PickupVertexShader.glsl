#version 330 core

layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec3 textCoord;
layout(location = 2) in float lightIntensity;

uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uModel;

out float vLightIntensity;
out vec3 vTextCoord;

void main()
{
	gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
	vTextCoord = textCoord;
	vLightIntensity = lightIntensity;
}