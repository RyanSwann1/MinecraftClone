#version 330 core

out vec4 color;
uniform sampler2DArray uTexture;

in vec3 vTextCoord;

void main()
{
	vec4 texColor = texture(uTexture, vTextCoord);
	if (texColor.a < 0.1)
	{
		discard;
	}

	color = texColor;
};