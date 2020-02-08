#version 330 core

out vec4 color;
uniform sampler2D uTexture;

in vec2 vTextCoord;

void main()
{
	color = vec4(0.2, 0.0, 0.0, 1.0);
	vec4 textColor = texture(uTexture, vTextCoord);
	color = textColor;
};