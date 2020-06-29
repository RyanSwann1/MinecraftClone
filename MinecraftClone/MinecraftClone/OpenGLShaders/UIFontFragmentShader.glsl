#version 330 core

out vec4 color;
uniform sampler2D uTexture;

in vec2 vTextCoord;

void main()
{
	color = texture(uTexture, vTextCoord);
	if (color == vec4(0.0, 0.0, 0.0, 1.0) ||
		color.a == 0)
	{
		discard;
	}
};