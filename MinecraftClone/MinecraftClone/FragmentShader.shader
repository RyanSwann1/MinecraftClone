#version 330 core

out vec4 color;
uniform sampler2D uTexture;
uniform float uAlpha;

in vec2 vTextCoord;

void main()
{
	vec4 textColor = texture(uTexture, vTextCoord);
	color = vec4(textColor.xyz, uAlpha);
};