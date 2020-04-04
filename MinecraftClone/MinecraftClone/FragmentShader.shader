#version 330 core

out vec4 color;
uniform sampler2DArray uTexture;
uniform float uAlpha;

in vec3 vTextCoord;

void main()
{
	vec4 textColor = texture(uTexture, vTextCoord);
	color = vec4(textColor.xyz, uAlpha);
};