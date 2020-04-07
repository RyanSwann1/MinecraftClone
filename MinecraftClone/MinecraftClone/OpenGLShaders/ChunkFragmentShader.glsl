#version 330 core

out vec4 color;
uniform sampler2DArray uTexture;

in vec3 vTextCoord;
in float vlightIntensity;

void main()
{
	color = vlightIntensity * texture(uTexture, vTextCoord);
	if (color.a == 0)
	{
		discard;
	}
};